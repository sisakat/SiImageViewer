#ifndef SIIMAGEVIEWER_H
#define SIIMAGEVIEWER_H

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector4D>

class SiImageViewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SiImageViewer(QWidget *parent = nullptr);
    ~SiImageViewer();

    void openImage(const QImage& image);
    void setBackground(const QColor& color);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_shaderProgram;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ibo;
    GLuint m_texture;
    GLuint m_textureLocation;
    GLuint m_mvpLocation;

    QImage m_image;
    QColor m_backgroundColor;

    QMatrix4x4 m_pre;
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_mvp;
    QVector2D m_cursorPos;
    QVector2D m_zoomPos;
    QVector2D m_imagePosition;
    QVector2D m_originalMousePos;
    bool m_setCursorFromZoom;
    bool m_panning;
    float m_scale{1.0};
    float m_zoomStep;

    bool m_update;

    void setupShaders();
    void setupBuffers();
    void setupTexture();
    void setupMatrices();
    void updateMatrices();

    QVector2D currentCursorPos() const;
    QVector2D screenToImage(const QVector2D& screen);
    QVector2D imageToScreen(const QVector2D& image);
};

#endif // SIIMAGEVIEWER_H
