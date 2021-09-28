# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

## [Unreleased]

## [1.1] - 2021-XX-XX

### Added
- Built-in implementation of Git version control plugin.
- A `CommandLineParser` class which allows to parse arguments from `OS.get_cmdline_args()`.
- An experimental support for cross-language mixin using `MixinScript` (aka `MultiScript`).
- A `PolyPath2D` node, which takes `Path2D` nodes to buffer curves into polygons.
- A `PolyCapsule2D` node, which generates capsule polygon.
- A `Stopwatch` node, which complements Godot's `Timer` node.
- A `ImageFrames.save_gif()` method which allows to save both static and animated GIF images.
- A `geometry` option/component, which is separate from `math` now.
- Numerous `GoostEngine` methods to retrieve copyright/license information, such as `get_license_text()`.
- "About Goost" editor dialog, which lists Goost authors, Goost license, and third-party licenses.

### Changed
- Refactor the process of configuring components and classes. You can use `python goost.py config` to configure components and individual classes now.
- Moved `PolyCollisionShape2D` class to the `physics` component.
- Converted `PolyBoolean/Offset/Decomp2D` classes into `Resource`s, as needed by `PolyPath2D`.
- Updated third-party image libraries.
- `ImageFrames.add_frame()` no longer accepts index to override existing frames.

### Removed
- Redundant and buggy `ListNode.erase()` method. You can safely use `ListNode.free()` regardless of whether a node was instantiated manually or via `LinkedList.push_back()`.
- No longer relevant `LinkedList.remove(ListNode)` method due to the above.

### Fixed
- Crashes with `LinkedList` when dealing with invalid data.
  - `insert_before/after(null, value)` no longer pushes front/back an element.
- Out of memory error when calling `GoostGeometry2D.simplify_polyline()` with `epsilon = 0`.
- Crashes while decomposing empty polygons with `PolyDecomp2D` when using `polypartition` geometry backend.
- Crash when attempting to load invalid GIF data from buffer using `ImageFrames.load_gif_from_buffer()`.
- Memory leaks in the image component.
- Vertex generation order in `GoostGeometry2D.regular_polygon()`.

## [1.0] - 2021-05-24

Initial release.

[Unreleased]: https://github.com/goostengine/goost/compare/1.0-stable+3.3.2...HEAD
[1.0]: https://github.com/goostengine/goost/compare/1.0-beta-gd3...1.0-stable+3.3.2
