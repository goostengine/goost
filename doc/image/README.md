# Image

The component provides `GoostImage` singleton for easy image processing. It
aims to build on top of Godot's built-in `Image` class to cater specific user
needs.

### List of public methods:
| Method              | Short Description                                                                    |
| ------------------- | ------------------------------------------------------------------------------------ |
| `replace_color`     | Replaces all occurrences of a given color with another one within an image.          |
| `bucket_fill`       | Fills the area with a color confined by opaque pixels.                               |
| `resize_hqx`        | Expands the image using either HQ2X or HQ3X algorithms.                              |
| `rotate`            | Rotates the image around its center.                                                 |
| `rotate_90`         | Rotates the image by 90 degrees in either direction.                                 |
| `rotate_180`        | Rotates the image by 180 degrees.                                                    |
| `binarize`          | Converts the image to binary (grayscale) with some threshold.                        |
| `dilate`            | Grows bright regions of the image.                                                   |
| `erode`             | Shrinks bright regions of the image.                                                 |
| `morph`             | Performs basic image morphological operations (dilation, erosion, closing, opening). |
| `tile`              | Tiles an image over a region.                                                        |
| `repeat`            | Tiles an image by repeating it a number of times in both dimensions.                 |
| `get_centroid`      | Finds a geometrical center of the image.                                             |
| `render_polygon`    | Renders a binary representation of the polygon as a new image.                       |
| `has_pixel`         | Tells whether a pixel lies inside image coordinates.                                 |
| `get_pixel_or_null` | Returns a pixel at specified image coordinates, or null if doesn't exist.            |

### List of classes:
| Class                                     | Short Description                                                             |
| ----------------------------------------- | ----------------------------------------------------------------------------- |
| [`ImageIndexed`](classes/ImageIndexed.md) | Adds pseudo support for indexed images with a color palette.                  |
| `ImageBlender`                            | Provides advanced image blending methods (derived from OpenGL specification). |
