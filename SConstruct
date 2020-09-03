#!/usr/bin/env python
#
# This is a convenience/compatibility SConstruct which allows to build Goost in 
# the same way as building Godot Engine, so the usage should be the same:
#
#     scons target=release_debug tools=yes bits=64
#
# Requirements:
# - Godot Engine source code can be found in the parent directory, OR
# - GODOT_SOURCE_PATH env var is defined pointing to Godot repository.
#
# Caveats/Limitations:
# - You have to switch to the relevant git branch in Godot repository yourself.
# - The `custom_modules` option is overridden to build Goost and accompanying
#   modules, so you cannot use this option to build other modules (currently).
# - The `extra_suffix` option is also overridden.
#
import os
import sys
import pickle
import subprocess

import goost
import version

# Avoid issues when building with different versions of Python.
SConsignFile(".sconsign{0}.dblite".format(pickle.HIGHEST_PROTOCOL))

# Find a path to Godot source to build with this module.
# Try relative path first.
godot_path = Dir("../godot")
if not godot_path.exists():
    # Try environment variable.
    godot_path = Dir(os.getenv("GODOT_SOURCE_PATH"))
    if not godot_path.exists():
        print("No path found to Godot Engine source.")
        print("Make sure to specify GODOT_SOURCE_PATH environment variable.")
        Exit()

def run_command(args):
    if sys.platform == "win32":
        subprocess.run(args, check=True, shell=True, cwd=godot_path.abspath)
    elif sys.platform == "linux":
        # This may not actually work on Linux systems, but works on WSL.
        subprocess.run(args, check=True, cwd=godot_path.abspath)

# Setup base SCons arguments (just copy all from the command line).
build_args = ["scons"]
for arg in ARGLIST:
    opt = "%s=%s" % (arg[0], arg[1])
    build_args.append(opt)

# Link this module as a custom module.
modules = [
    # We cannot link to a single module. As a byproduct, this may compile other 
    # modules which reside in the same location as this module.
    Dir("..").abspath,
    # This module provides built-in and community modules, just like this one.
    os.path.join(Dir(".").abspath, "modules")
]
build_args.append("custom_modules=%s" % ",".join(modules))

# Append extra suffix to distinguish between other Godot builds.
build_args.append("extra_suffix=%s" % version.short_name)

# Extend build name to the module name, preserving any custom build names.
build_name = os.environ["BUILD_NAME"]
if build_name:
    build_name = version.short_name.capitalize() + "." + build_name
os.environ["BUILD_NAME"] = build_name

print("Building Godot with Goost ...")
run_command(build_args)
