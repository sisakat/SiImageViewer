#include "siimageviewer.h"

#include <QMouseEvent>
#include <QtMath>

const float DEFAULT_ZOOM_STEP = 1.50f;
const float FINE_ZOOM_STEP    = 1.05f;

const char* VERTEX_SHADER =
    "#version 330                            \n"
    "layout(location = 0) in vec4 vtx_pos  ; \n"
    "layout(location = 1) in vec2 vtx_txpos; \n"
    "out vec2 texcoord;                      \n"
    "uniform mat4 mvp;                       \n"
    "void main() {                           \n"
    "   texcoord = vtx_txpos;                \n"
    "   gl_Position = mvp * vtx_pos;         \n"
    "}                                       \n";

const char* FRAGMENT_SHADER =
    "#version 330                            \n"
    "uniform sampler2D tex;                  \n"
    "in vec2 texcoord;                       \n"
    "layout(location = 0) out vec4 FragColor;\n"
    "void main() {                           \n"
    "   FragColor = texture(tex, texcoord);  \n"
    "}                                       \n";


SiImageViewer::SiImageViewer(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::WheelFocus);

    QSurfaceFormat format;
    format.setVersion(3,3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    m_zoomStep = DEFAULT_ZOOM_STEP;
}

SiImageViewer::~SiImageViewer()
{
    glDeleteTextures(1, &m_texture);

    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);

    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteProgram(m_shaderProgram);
}

void SiImageViewer::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_RGBA8888);

    unsigned char* imageData = m_image.bits();
    int width = m_image.width();
    int height = m_image.height();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    setupMatrices();
    update();
}

void SiImageViewer::setBackground(const QColor &color)
{
    m_backgroundColor = color;
}

void SiImageViewer::reset()
{
    setupMatrices();
}

void SiImageViewer::rotate(float angleDeg)
{
    m_model.translate(m_image.width()/2.0f, m_image.height()/2.0f);
    m_model.rotate(-angleDeg, 0.0f, 0.0f, 1.0f);
    m_model.translate(-m_image.width()/2.0f, -m_image.height()/2.0f);
}

void SiImageViewer::rotateAround(float angleDeg, const QPoint &point)
{
    m_model.translate(point.x(), point.y());
    m_model.rotate(-angleDeg, 0.0f, 0.0f, 1.0f);
    m_model.translate(-point.x(), -point.y());
}

void SiImageViewer::translate(float x, float y)
{
    m_model.translate(x, y);
}

void SiImageViewer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setupShaders();
    setupBuffers();
    setupTexture();
    setupMatrices();
}

void SiImageViewer::paintGL()
{
    m_cursorPos = screenToImage(currentCursorPos());
    updateMatrices();
    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(m_textureLocation, 0);
    glUniformMatrix4fv(m_mvpLocation, 1, GL_FALSE, m_mvp.data());
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SiImageViewer::resizeGL(int width, int height)
{
    update();
}

void SiImageViewer::mousePressEvent(QMouseEvent *event)
{
    m_originalMousePos = currentCursorPos();
    m_mouseDownPos = currentCursorPos();
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
    }
    update();
}

void SiImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
    }
    update();
}

void SiImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    auto currentPos = QVector2D{event->pos().x() * 1.0f, event->pos().y() * 1.0f};
    if (m_panning) {
        auto oldP = screenToImage(m_originalMousePos);
        auto newP = screenToImage(currentPos);
        auto tran = newP - oldP;
        m_model.translate(tran.x(), tran.y());
        m_originalMousePos.setX(event->pos().x());
        m_originalMousePos.setY(event->pos().y());
    } else if (m_shiftDown && m_rDown) {
        auto oldP = m_originalMousePos;
        auto newP = currentPos;
        auto tran = newP - oldP;
        auto imagePos = screenToImage(m_mouseDownPos);
        rotateAround(-tran.y(), QPoint(imagePos.x(), imagePos.y()));
        m_originalMousePos.setX(event->pos().x());
        m_originalMousePos.setY(event->pos().y());
    } else if (!m_shiftDown && m_rDown) {
        auto oldP = m_originalMousePos;
        auto newP = currentPos;
        auto tran = newP - oldP;
        if (std::abs(tran.y()) > this->devicePixelRatioF() * 30) {
            if (tran.y() > 0) {
                rotate(-90);
            } else {
                rotate(90);
            }
            m_originalMousePos.setX(event->pos().x());
            m_originalMousePos.setY(event->pos().y());
        }
    }
    update();
}

void SiImageViewer::wheelEvent(QWheelEvent *event)
{
    m_zoomPos = screenToImage(currentCursorPos());
    if (event->angleDelta().y() > 0) {
        m_scale *= m_zoomStep;
    } else if (event->angleDelta().y() < 0) {
        m_scale *= 1.0f / m_zoomStep;
    }
    update();
    m_setCursorFromZoom = true;
}

void SiImageViewer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        m_shiftDown = true;
        m_zoomStep = FINE_ZOOM_STEP;
    } else if (event->key() == Qt::Key_Control) {
        m_ctrlDown = true;
    } else if (event->key() == Qt::Key_R) {
        m_rDown = true;
    }

    if (m_ctrlDown && m_rDown) {
        reset();
    }
    update();
}

void SiImageViewer::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        m_shiftDown = false;
        m_zoomStep = DEFAULT_ZOOM_STEP;
    } else if (event->key() == Qt::Key_Control) {
        m_ctrlDown = false;
    } else if (event->key() == Qt::Key_R) {
        m_rDown = false;
    }
    update();
}

void SiImageViewer::setupShaders()
{
    const char* source;
    int length;
    GLint status;

    source = VERTEX_SHADER;
    length = strlen(VERTEX_SHADER);
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &source, &length);
    glCompileShader(m_vertexShader);
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not compile vertex shader.");
    }

    source = FRAGMENT_SHADER;
    length = strlen(FRAGMENT_SHADER);
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &source, &length);
    glCompileShader(m_fragmentShader);
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not compile fragment shader.");
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not link shaders.");
    }
}

void SiImageViewer::setupBuffers()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    GLfloat vertexData[] = {
        //  x     y     z     u     v
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,0.0f, 0.0f, 1.0f, 1.0f,
        0.0f,0.0f, 0.0f, 0.0f, 1.0f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

    // generate and bind the index buffer object
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    GLuint indexData[] = {
        0,1,2, // first triangle
        2,1,3, // second triangle
    };

    // fill with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);

    // cleanup
    glBindVertexArray(0);
}

void SiImageViewer::setupTexture()
{
    // generate texture
    glGenTextures(1, &m_texture);

    // bind the texture
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_textureLocation = glGetUniformLocation(m_shaderProgram, "tex");
}

void SiImageViewer::setupMatrices()
{
    m_pre.setToIdentity();
    m_model.setToIdentity();
    m_view.setToIdentity();
    m_projection.setToIdentity();
    m_mvpLocation = glGetUniformLocation(m_shaderProgram, "mvp");
}

void SiImageViewer::updateMatrices()
{
    m_pre.setToIdentity();
    // m_model.setToIdentity();
    m_view.setToIdentity();
    QMatrix4x4 model = m_model;

    if (!m_image.isNull()) {
        float imageAspect = 1.0f * m_image.height() / m_image.width();
        float windowAspectX = 1.0f * this->height() / this->width();

        float width = (1.0f *  2.0f / m_image.width()) * windowAspectX * 1.0f/imageAspect;
        float height = (1.0f * 2.0f / m_image.height());

        m_pre.scale(m_image.width(), m_image.height());

        m_view.scale(width, height);
        m_view.translate(-m_image.width()/2.0f, -m_image.height()/2.0f, 0.0f);

        m_model.translate(m_cursorPos.x(), m_cursorPos.y());
        m_model.scale(m_scale);
        m_model.translate(-m_cursorPos.x(), -m_cursorPos.y());
        m_scale = 1.0;
    }

    // m_model = model;
    m_mvp = m_projection * m_view * m_model * m_pre;
}

QVector2D SiImageViewer::currentCursorPos() const
{
    auto pos = mapFromGlobal(QCursor::pos());
    return {pos.x() * 1.0f, pos.y() * 1.0f};
}

QVector2D SiImageViewer::screenToImage(const QVector2D &screen)
{
    // viewport transformation
    float x = 2.0f * screen.x() / this->width() - 1.0f;
    float y = 2.0f * (this->height() - screen.y() - 1) / this->height() - 1.0f;

    // apply inverse transformation to get to image pixel coordinates
    QVector4D vec(x, y, 1.0f, 1.0f);
    vec = m_model.inverted() * m_view.inverted() * m_projection.inverted() * vec;

    return {vec.x(), vec.y()};
}

QVector2D SiImageViewer::imageToScreen(const QVector2D &image)
{
    QVector4D vec(image.x() / m_image.width(), image.y() / m_image.height(), 1.0f, 1.0f);
    vec = m_mvp * vec;
    float x = (vec.x() + 1.0f) / 2.0 * this->width();
    float y = this->height() - 1 - (vec.y() + 1.0f) / 2.0 * this->height();
    return {x, y};
}
