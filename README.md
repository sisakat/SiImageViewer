# Qt Image Viewer
Simple Qt Image Viewer Widget with support for panning, zoom and rotations.
Implemented as an QOpenGLWidget.

## Usage
Follow these instructions to embed the image viewer into your project:

1. Add `siimageviewer.h` and `siimageviewer.cpp` to your project.
2. Create an empty widget and promote it to `SiImageViewer`.
3. Call `setImage(const QImage& image)` to set the current image.

All you need to show an image is an `QImage` instance, which can be created from memory or file.

## Shortcuts
`Scroll Wheel` for fast zooming.

`Shift + Scroll Wheel` for precise zooming.

`Scroll Wheel Press` for panning.

`R + Left Mouse Button` for rotation in 90 degree steps around the image center.

`Shift + R + Left Mouse Button` for precise rotation around the cursor.

`Ctrl + R` to rotate image 90 degrees around the image center.

`R` to reset all transforms.

