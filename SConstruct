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
# - You have to switch to the relevant git branch in Godot repository yourself
#   if you use GODOT_SOURCE_PATH or relative path, else done automatically.
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
godot_search_dirs = [
    Dir(os.getenv("GODOT_SOURCE_PATH")), # Try environment variable first.
    Dir("godot"), # The engine might already be checked out.
    Dir("../godot"), # Try relative path last.
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
        # Checkout the engine directly into Goost.
        run_command(["git", "clone", "--depth=1", version.godot_url, 
                "-b", version.godot_version, "--single-branch"])
    else:
        run_command(["git", "reset", "--hard", version.godot_version],
                godot_dir.abspath)

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
build_name = version.short_name.capitalize()
if os.getenv("BUILD_NAME"):
    build_name += "." + os.getenv("BUILD_NAME")
os.environ["BUILD_NAME"] = build_name

# Enable cache if required, this is reused by Godot.
scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("SCons cache enabled... (path: '" + scons_cache_path + "')")

print("Building Godot with Goost ...")
run_command(build_args, godot_dir.abspath)
