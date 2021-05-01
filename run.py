#!/usr/bin/python3

import os
import sys
import argparse
import subprocess


def get_engine_executable_path():
    import json
    binary_path = ""
    try:
        with open("godot/.scons_env.json", "r") as f:
            env = json.load(f)
            if env["platform"] == "server":
                binary_path = "godot/bin/godot_server" + env["PROGSUFFIX"]
            else:
                binary_path = "godot/bin/godot" + env["PROGSUFFIX"]
    except OSError as e:
        print(e)
    if not binary_path:
        raise ValueError("No engine construction environment found.")
    return binary_path


if __name__ == "__main__":
    if not os.path.exists("godot"):
        print("Error: no Godot repository found at Goost root, aborting.")
        print("Please run `scons` command first to clone Godot repository from remote and build Godot with Goost.")
        sys.exit(255)

    base_path = os.path.dirname(os.path.abspath(__file__))
    try:
        godot_bin = get_engine_executable_path()
    except ValueError as e:
        print(e)
        print("Have you forgot to build the engine first?")
        sys.exit(255)

    parser = argparse.ArgumentParser()
    parser.add_argument("tool", help='The type of a tool to run: "editor", "tests", "doctool".')
    args = parser.parse_args()

    if args.tool.startswith("editor"):
        print("Running Godot editor ...")
        ret = subprocess.run([
                godot_bin, "--path", "tests/project", "--editor"]).returncode
        sys.exit(ret)
    elif args.tool.startswith("test"):
        print("Running Goost tests ...")
        ret = subprocess.run([
                godot_bin, "--path", "tests/project", "-d", "-s",
                os.path.join(base_path, "tests/project/addons/gut/gut_cmdln.gd")]).returncode
        sys.exit(ret)
    elif args.tool.startswith("doc"):
        print("Generating documentation ...")
        os.makedirs("doc/godot")
        ret = subprocess.run([
                godot_bin, "--doctool",
                os.path.join(base_path, "doc/godot")]).returncode
        sys.exit(ret)
    else:
        print('Error: tool not found. Run with `--help` to list available tools.')
