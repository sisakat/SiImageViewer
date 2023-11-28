# Qt Image Viewer
Simple Qt Image Viewer Widget with support for panning, zoom and rotations.
Implemented as an QOpenGLWidget.

## Usage
Follow these instructions to embed the image viewer into your project:

1. Add `siimageviewer.h` and `siimageviewer.cpp` to your project.
2. Create an empty widget and promote it to `SiImageViewer`.
3. Call `setImage(const QImage& image)` to set the current image.

All you need to show an image is a `QImage` instance, which can be created from memory or file.

## Shortcuts

| Shortcut                          | Description                    |
| --------------------------------- | ------------------------------ |
| `Scroll Wheel`                    | Fast zooming                   |
| `Shift + Scroll Wheel`            | Precise zooming                |
| `Scroll Wheell Press`             | Panning                        |
| `Ctrl + R`                        | Rotation in 90 degree steps    | 
| `R + Left Mouse Button`           | Rotation in 90 degree steps    |
| `Shift + R + Left Mouse Button`   | Precise rot. around cursor     |
| `R`                               | Reset all transformations      |

