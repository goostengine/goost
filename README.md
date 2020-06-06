![Goost](logo.png)

# Goost: Godot Engine Extension

[![Build Status](https://travis-ci.com/GoostGD/goost.svg?branch=gd3)](https://travis-ci.com/GoostGD/goost)
[![Build status](https://ci.appveyor.com/api/projects/status/github/GoostGD/goost?branch=gd3&svg=true)](https://ci.appveyor.com/project/Xrayez/goost/branch/gd3)

This is a [Godot Engine](https://github.com/godotengine/godot) extension
implemented as a single-entry C++ module.

## Overview

The current feature set of this extension can be summarized as a list of
functional core components. To learn more about them, proceed to the links
below:

| Method                             | Notable features                                                                   |
| ---------------------------------- | ---------------------------------------------------------------------------------- |
| [Image](doc/image/README.md)       | Image processing methods, indexed image support.                                   |
| [Geometry](doc/geometry/README.md) | Geometry methods to perform polygon boolean operations, offsetting, decomposition. |
| [Physics](doc/physics/README.md)   | Shape casting class.                                                               |

There are also some modules which can be compiled alongside of the extension,
see below for details.

## Installation

Before installing, you must be able to 
[compile Godot Engine](https://docs.godotengine.org/en/latest/development/compiling/) 
from source. Once ready, build the engine with Goost:

```sh
git clone https://github.com/GoostGD/goost.git
scons custom_modules="/path/to/directory/containing/goost"
```

Once the compilation phase is done, run the executable and search the built-in
documentation pages to make sure that the classes provided by the extension
are instantly accessible.

### Configuring the build

#### Core components

Core components are structure-agnostic, functional parts of the extension. By
default, all components are built, but it's also possible to disable them.

Disabling a single component involves compiling the engine with one of the
`goost_*_enabled` build options:

```sh
scons custom_modules="/path/to/dir/containing/goost" goost_math_enabled=no
```

For the full list of components, see [core.py](core.py).

#### Module components

The extension provides as set of optional modules (regular C++ modules just like
this extension) which are disabled by default. In order to compile them, you can
append to the list of paths specified by `custom_modules` option:

```sh
scons custom_modules="/path/to/dir/containing/goost,/path/to/goost/modules"
```

It's possible to compile the modules independently of whether Goost is enabled:

```sh
scons module_goost_enabled="no" custom_modules="/path/to/goost/modules"
```

But bear in mind that some built-in modules may rely on one of the core
components listed above.

For more advanced module-related options, see [Modules](modules/README.md).

---

For other Goost-specific build options, run `scons --help | grep goost_`.

For more information regarding `custom_modules` and other build options, see
official documentation:
- [Introduction to the build system: Custom modules](https://docs.godotengine.org/en/latest/development/compiling/introduction_to_the_buildsystem.html#custom-modules)
- [Custom modules in C++](https://docs.godotengine.org/en/latest/development/cpp/custom_modules_in_cpp.html)

### Compatibility

Godot Engine 3.2 compatible.

`custom_modules` build option is added since the release of Godot Engine 3.2.2.

## Development

See [Contributing](CONTRIBUTING.md) if you are interested in development and to
find out what kind of features are accepted.

### Philosophy

As you may have already noticed, the extension's source tree structure closely
resembles Godot Engine's structure. In fact, you can see this extension as a
mini-engine which builds on top of the existing core functionality of Godot, as
most features are easily accessible within a module, which is the **extension**
in the Goost terms. Given this knowledge, Goost tries to follow most conventions
which are accepted for Godot Engine development.

Components are developed in terms of functional value. It's the responsibility
of Goost developers to make the necessary internal structural changes to
maintain this user expectation.

Similarly, most classes and methods are broken down into their own `2d` and `3d`
subfolders, so that either 2D or 3D components can be easily disabled.

### Creating or adding new modules

The extension provides optional C++ modules which can be compiled alongside,
and has mostly convenience purpose of "batteries included".

See [Modules](modules/README.md) for details.

### Testing

While not mandatory, developers are encouraged to write unit tests (or any tests
for that matter) for any existing or new features.

See [Tests](tests/README.md) for details.

### Documentation

The documentation can be generated for new classes, and updated for existing
ones. See the following official documentation pages for further instructions:

- [Contribute to the Class Reference](https://docs.godotengine.org/en/latest/community/contributing/updating_the_class_reference.html)
- [Writing custom documentation](https://docs.godotengine.org/en/latest/development/cpp/custom_modules_in_cpp.html#writing-custom-documentation)

### Versioning

There are separate branches which are targeted for specific Godot Engine's major
version.

Regarding latest extension development version, switch to
* [`gd3`](https://github.com/GoostGD/goost/tree/gd3) branch for Godot 3.2+
* [`gd4`](https://github.com/GoostGD/goost/tree/gd4) branch for Godot 4.x

Similarly, you may use any of the stable branches with a similar branch
encoding, such as `1.0-gd3` or `1.0-gd4`, if available.

Any new features are encouraged to be developed against the latest Godot
Engine's *stable* version, which may be optionally ported to the latest engine's
*development* version (independently or in sync, depending on priorities).

## License

The extension is released under the [MIT license](LICENSE.md).

See the full list of third-party libraries with their licenses at
[thirdparty/README.md](thirdparty/README.md) (MIT compatible).
