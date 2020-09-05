![Goost](logo.png)

# Goost: Godot Engine Extension

![🐧 Linux](https://github.com/goostengine/goost/workflows/%F0%9F%90%A7%20Linux/badge.svg)
![🍎 macOS](https://github.com/goostengine/goost/workflows/%F0%9F%8D%8E%20macOS/badge.svg)
![🎨 Windows](https://github.com/goostengine/goost/workflows/%F0%9F%8E%A8%20Windows/badge.svg)
![🤖 Android](https://github.com/goostengine/goost/workflows/%F0%9F%A4%96%20Android/badge.svg)
![🍏 iOS](https://github.com/goostengine/goost/workflows/%F0%9F%8D%8F%20iOS/badge.svg)
![🌐 JavaScript](https://github.com/goostengine/goost/workflows/%F0%9F%8C%90%20JavaScript/badge.svg)

This is a [Godot Engine](https://github.com/godotengine/godot) 3.2+ extension
implemented as a C++ module.

The extension is comprised of functional components and other modules, most of
which should be optional to build.

See [Goost official documentation](https://goost.readthedocs.io/en/gd3/) for
detailed instructions on how to use the module, and the list of currently
implemented components.

## Compiling

```sh
git clone https://github.com/goostengine/goost
cd goost
scons
```

This will clone the Godot Engine repository and compile the engine with Goost.
You'll find the resulting binaries at `godot/bin` directory relative to the
Goost repository.

For more advanced compilation options, refer to
[Goost Installation](https://goost.readthedocs.io/en/gd3/usage/installation.html)
documentation pages.

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

### Versioning and compatibility

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
[MIT license](LICENSE.txt).

See the full list of third-party libraries with their licenses used by this
extension at [thirdparty/README.md](thirdparty/README.md) (MIT compatible).
