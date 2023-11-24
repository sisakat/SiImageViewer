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

    /**
     * @brief Sets the main image.
     * @param image Image to display.
     */
    void setImage(const QImage& image);

    /**
     * @brief Sets the background color of the viewer.
     * @param color Background color
     */
    void setBackground(const QColor& color);

    /**
     * @brief Resets all user actions (rotations, translations, zoom).
     */
    void reset();

    /**
     * @brief Rotates the image counter-clockwise around the image center.
     * @param angleDeg Angle in degrees.
     */
    void rotate(float angleDeg);

    /**
     * @brief Rotates the image counter-clockwise around the given point.
     * @param angleDeg Angle in degrees.
     * @param point Rotation center
     */
    void rotateAround(float angleDeg, const QPoint& point);

    /**
     * @brief Translates the image x, y amount of pixels.
     * @param x Amount in x direction.
     * @param y Amount in y direction.
     */
    void translate(float x, float y);

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

    // Unifrom locations
    GLuint m_textureLocation;
    GLuint m_mvpLocation;

    int32_t m_imageWidth{1};
    int32_t m_imageHeight{1};
    QColor m_backgroundColor;

    QMatrix4x4 m_pre;        // used to transform the vertex coordinates to match the image dimension
    QMatrix4x4 m_model;      // used for global transformations (user rotation, scaling, ...)
    QMatrix4x4 m_view;       // used for viewport transformation
    QMatrix4x4 m_projection; // not in use
    QMatrix4x4 m_mvp;        // multiplication of projection, view, model, pre
    QVector2D m_cursorPosImage;   // cursor position in image coordinates
    QVector2D m_originalMousePos; // cursor position on first mouse down
    QVector2D m_mouseDownPos;     // cursor position from mouse down event

    float m_scale{1.0}; // current scaling amount to apply
    float m_zoomStep;   // scaling factor for one scroll wheel change

    bool m_panning;   // true when middle mouse button is held down
    bool m_shiftDown; // true when shift is held down
    bool m_ctrlDown;  // true when control is held down
    bool m_rDown;     // true when R is held down

    void setupShaders();
    void setupBuffers();
    void setupTexture();
    void setupMatrices();
    void updateMatrices();

    /**
     * @brief Gets the current cursor position relative to the widget.
     * @return Relative cursor position
     */
    QVector2D currentCursorPos() const;

    /**
     * @brief Unprojects an screen point back to the image.
     * @param screen A point in screen coordinates.
     * @return Image coordinates in pixels.
     */
    QVector2D screenToImage(const QVector2D& screen);

    /**
     * @brief Projects an image point onto the screen.
     * @param image A point in image coordinates (pixels).
     * @return Screen coordinates in pixels.
     */
    QVector2D imageToScreen(const QVector2D& image);
};

#endif // SIIMAGEVIEWER_H
