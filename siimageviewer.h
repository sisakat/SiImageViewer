#ifndef SIIMAGEVIEWER_H
#define SIIMAGEVIEWER_H

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QMatrix4x4>

class SiImageViewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SiImageViewer(QWidget *parent = nullptr);
    ~SiImageViewer();

    void openImage(const QImage& image);

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

    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_mvp;

    bool m_update;

    void setupShaders();
    void setupBuffers();
    void setupTexture();
    void setupMatrices();

    void updateMatrices();
};

#endif // SIIMAGEVIEWER_H
