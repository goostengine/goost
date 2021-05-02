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


def run(p_args, windowed=False): # Assumes the first arg is the binary path.
    args = p_args.copy()
    if not windowed:
        if sys.platform.startswith("win"):
            args.insert(1, "--no-window") # Only implemented on Windows.
    return subprocess.run(args).returncode


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

    # Unit test arguments.
    parser.add_argument("-t", "--test-file",
            help='A relative path to test file to run, for instance: "core/math/test_random.gd"')
    parser.add_argument("-tc", "--test-case",
            help="Name of a test case to run. Any test case matching the name will be run.")

    args = parser.parse_args()

    if args.tool.startswith("editor"):
        print("Running Godot editor ...")
        ret = run([godot_bin, "--path", "tests/project", "--editor"], windowed=True)
        sys.exit(ret)
    elif args.tool.startswith("test"):
        print("Running Goost tests ...")
        test_args = [godot_bin, "--path", "tests/project", "-d", "-s",
                os.path.join(base_path, "tests/project/addons/gut/gut_cmdln.gd")]
        if args.test_file:
            # Reset `-gdir`, otherwise all scripts are going to be collected
            # recursively from the directory defined in `.gutconfig.json`.
            res_file = "res://goost/" + args.test_file
            test_args.extend(["-gtest=%s" % res_file, '-gdir='])
        if args.test_case:
            test_args.extend(["-gunit_test_name=%s" % args.test_case])
        ret = run(test_args)
        sys.exit(ret)
    elif args.tool.startswith("doc"):
        print("Generating documentation ...")
        if not os.path.exists("doc/godot"):
            os.makedirs("doc/godot")
        ret = run([godot_bin, "--doctool", os.path.join(base_path, "doc/godot")])
        sys.exit(ret)
    else:
        print('Error: tool not found. Run with `--help` to list available tools.')
