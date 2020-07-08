![Goost](logo.png)

# Goost: Godot Engine Extension

[![Build Status](https://travis-ci.com/GoostGD/goost.svg?branch=gd3)](https://travis-ci.com/GoostGD/goost)
[![Build status](https://ci.appveyor.com/api/projects/status/github/GoostGD/goost?branch=gd3&svg=true)](https://ci.appveyor.com/project/Xrayez/goost/branch/gd3)

This is a [Godot Engine](https://github.com/godotengine/godot) extension
implemented as a C++ module.

The extension is comprised of functional components, all of which are optional.
There are also some modules which can be compiled alongside of the extension.

See [Goost official documentation](https://goost.readthedocs.io/en/gd3/) for
detailed instructions on how to use the module, and the list of currently
implemented components.

## Compiling

```sh
git clone https://github.com/goostengine/goost.git
scons custom_modules="/path/to/directory/containing/goost"
```

### Compatibility

Godot Engine 3.2 compatible.

`custom_modules` build option is added since the release of Godot Engine 3.2.2.

## Development

See [Contributing](CONTRIBUTING.md) if you are interested in development and to
find out what kind of features are accepted.

### Writing tests

While not mandatory, developers are encouraged to write unit tests (or any tests
for that matter) for any existing or new features.

See [Tests](tests/README.md) for details.

### Writing documentation

The documentation can be generated for new classes, and updated for existing
ones. See the following official documentation pages for further instructions:

- [Contribute to the Class Reference](https://docs.godotengine.org/en/latest/community/contributing/updating_the_class_reference.html)
- [Writing custom documentation](https://docs.godotengine.org/en/latest/development/cpp/custom_modules_in_cpp.html#writing-custom-documentation)

### Versioning

There are separate branches which are targeted for specific Godot Engine's major
version.

Regarding latest extension development version, switch to
* [`gd3`](https://github.com/goostengine/goost/tree/gd3) branch for Godot 3.2+
* [`gd4`](https://github.com/goostengine/goost/tree/gd4) branch for Godot 4.x (does not exist yet).

Similarly, you may use any of the stable branches with a similar branch
encoding, such as `1.0-gd3` or `1.0-gd4`, if available.

Any new features are encouraged to be developed against the latest Godot
Engine's *stable* version, which may be optionally ported to the latest engine's
*development* version (independently or in sync, depending on priorities).

## License

Unless otherwise specified, the extension is released under the
[MIT license](LICENSE.md).

See the full list of third-party libraries with their licenses used by this
extension at [thirdparty/README.md](thirdparty/README.md) (MIT compatible).
