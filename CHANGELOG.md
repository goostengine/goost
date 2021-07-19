# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

## [Unreleased]

## [1.1] - 2021-XX-XX

### Added
- Built-in implementation of Git version control plugin.
- An experimental support for cross-language mixin using `MixinScript` (aka `MultiScript`).
- A `Stopwatch` node, which complements Godot's `Timer` node.
- A `ImageFrames.save_gif()` method which allows to save both static and animated GIF images.
- A `geometry` option/component, which is separate from `math` now.
- Numerous `GoostEngine` methods to retrieve copyright/license information, such as `get_license_text()`.
- "About Goost" editor dialog, which lists Goost authors, Goost license, and third-party licenses.

### Changed
- Refactor the process of configuring components and classes. You can use `python goost.py config` to configure components and individual classes now.
- Moved `PolyCollisionShape2D` class to the `physics` component.
- Updated third-party image libraries.

### Fixed
- Crashes with `LinkedList` when dealing with invalid data.
  - `insert_before/after(null, value)` no longer pushes front/back an element.

## [1.0] - 2021-05-24

Initial release.

[Unreleased]: https://github.com/goostengine/goost/compare/1.0-stable+3.3.2...HEAD
[1.0]: https://github.com/goostengine/goost/compare/1.0-beta-gd3...1.0-stable+3.3.2
