#!/usr/bin/env python
#
# Upstream: https://github.com/goostengine/goost (see `SConstruct` file)
# Version: 1.0
# License: MIT
#
# This is a convenience/compatibility SConstruct which allows to build any 
# C++ module in the same way as building Godot Engine (3.2+).
#
# Installation:
#
# 1. Copy this SConstruct to your module's source root.
# 2. Compile the module with the same build command, for instance:
#
#     scons target=release_debug tools=yes bits=64
#
# This will clone the Godot repository directly into this module and compile it
# accordingly. Alternatively, this script will compile the engine externally if 
# the source code can be found in the parent directory or `GODOT_SOURCE_PATH` 
# environment variable is defined pointing to Godot source.
#
# Caveats/Limitations:
#
# - You have to switch to the relevant git branch in Godot repository yourself
#   if you use parent directory or `GODOT_SOURCE_PATH`, else done automatically.
# - The `custom_modules`, `extra_suffix`, `profile` build options are overridden
#   to build this module, so you cannot use these from the command-line.
#
import os
import sys
import pickle
import subprocess

import config

godot_version = os.getenv("GODOT_VERSION", "3.2") # A branch, commit, tag etc.
godot_url = os.getenv("GODOT_REPO_URL", "https://github.com/godotengine/godot")

# Module name is determined from directory name.
module_name = os.path.basename(Dir(".").abspath)

# Find a path to Godot source to build with this module.
godot_search_dirs = [
    Dir(os.getenv("GODOT_SOURCE_PATH")), # Try environment variable first.
    Dir("../godot"), # Try relative path.
    Dir("godot"), # Clone or use already checked out engine last.
]
godot_dir = Dir("godot")

for path in godot_search_dirs:
    if not path.exists():
        continue
    godot_dir = path
    break

def run_command(args, dir="."):
    if sys.platform.startswith("win32"):
        subprocess.run(args, check=True, shell=True, cwd=dir)
    else:
        subprocess.run(args, check=True, cwd=dir)

if godot_dir == Dir("godot"):
    if not godot_dir.exists():
        # Checkout Godot repository directly into this module.
        run_command(["git", "clone", godot_url, "-b", godot_version, "--single-branch"])
    else:
        run_command(["git", "reset", "--hard", godot_version], godot_dir.abspath)

# Setup base SCons arguments (just copy all from the command line).
build_args = ["scons"]
for arg in ARGLIST:
    opt = "%s=%s" % (arg[0], arg[1])
    build_args.append(opt)

# Link this module as a custom module.
modules = []
modules.append(Dir("..").abspath)

# This module may provide built-in and community modules, just like this one.
try:
    modules_path = config.get_modules_path()
    modules.append(os.path.join(Dir(".").abspath, modules_path))
except AttributeError:
    pass

build_args.append("custom_modules=%s" % ",".join(modules))

# We cannot link to a single module using the `custom_modules` build option,
# so this may compile other modules which reside in the same location as this 
# module. To prevent this, we disable all modules there, excluding this one.
if os.getenv("GOOST_PARENT_MODULES", "disabled") == "enabled":
    DIRNAMES = 1
    dirs = next(os.walk(Dir("..").abspath))[DIRNAMES]
    parent_modules = []

    for d in dirs:
        if d == module_name:
            continue
        if os.path.exists(os.path.join(Dir("..").abspath, d, "SCsub")):
            parent_modules.append(d)

    for m in parent_modules:
        build_args.append("module_%s_enabled=no" % m)

# Optionally disable Godot's built-in modules which are non-essential in order
# to test out this module in the engine. For more details, refer to Godot docs:
# https://docs.godotengine.org/en/latest/development/compiling/optimizing_for_size.html
if os.getenv("GODOT_BUILTIN_MODULES", "enabled") == "disabled":
    try:
        import modules_disabled
        build_args.append("profile=%s" % 
                os.path.join(Dir(".").abspath, "modules_disabled.py"))
    except ImportError:
        pass

# Append extra suffix to distinguish between other Godot builds.
build_args.append("extra_suffix=%s" % module_name)

# Extend build name to the module name, preserving any custom build names.
build_name = module_name.capitalize()
if os.getenv("BUILD_NAME"):
    build_name += "." + os.getenv("BUILD_NAME")
os.environ["BUILD_NAME"] = build_name

# Avoid issues when building with different versions of Python.
SConsignFile(".sconsign{0}.dblite".format(pickle.HIGHEST_PROTOCOL))

# Enable cache if required, this is reused by Godot and CI.
scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("SCons cache enabled... (path: '" + scons_cache_path + "')")

# Build the engine with the module.
print("Building Godot with %s ..." % module_name.capitalize())
run_command(build_args, godot_dir.abspath)
