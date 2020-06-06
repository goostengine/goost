# Modules

This directory contains both builtin and community modules. Builtin modules are
officially maintained by the Goost authors and are versioned as part of the
extension. Community modules represent `git` submodules which are maintained by
third-party developers.

The [`disable_builtin.py`](disable_builtin.py) and
[`disable_community.py`](disable_community.py) scripts aim to list all modules
in such a way that they can be disabled by users, as modules are enabled by
default once detected by the build system, unless they are explicitly disabled
via `config.py::is_enabled` method per each module.

If you'd like to opt-out from compiling certain modules which come with this
extension, you'll have to disable each of the unused modules explicitly:

```sh
scons custom_modules="/path/to/dir/containing/goost,/path/to/goost/modules" \
module_a_enabled="no" module_b_enabled="no" module_c_enabled="no" ...
```

All of the above options can be conveniently defined by creating `custom.py` at
the root of Godot source, or pointing to an existing configuration file such as
above:

```sh
scons profile="modules/goost/disable_community.py"
```

There may be also community modules which are present as `git` submodules.
In order to fetch them:

```sh
git submodule update --init --recursive
```

Or if you haven't yet cloned Goost:

```sh
git clone https://github.com/GoostGD/goost.git --recurse-submodules
```

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
