# Modules

This directory contains both builtin and community modules. Builtin modules are
officially maintained by the Goost authors and are versioned as part of the
extension. Community modules represent `git` submodules which are maintained by
third-party developers.

The [`disable_builtin.py`](disable_builtin.py) and
[`disable_community.py`](disable_community.py) scripts aim to list all modules
in such a way that they can be disabled by users (for instance with
`scons profile=disable_community.py`), as modules are enabled by default once
detected by the build system, unless they are explicitly disabled via
`config.py::is_enabled` method per each module.

## Adding community modules

The following command must be used from within the root of the Goost repository:

```sh
git submodule add --name module_name <URL> modules/module_name
```

The `--name` option is recommended to specify as cloning the module via the URL
may not always produce correct module name required by the build system. For
instance, if you have a module with URL similar to:

```sh
https://github.com/Godette/gd-awesome-module
```

You'll need to add the module with:

```sh
git submodule add --name awesome_module https://github.com/Godette/gd-awesome-module modules/awesome_module
```
