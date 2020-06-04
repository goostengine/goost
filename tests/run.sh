#!/usr/bin/env bash

# For reference, see:
# https://github.com/godotengine/godot-tests/blob/master/tests/project_export/test_project.sh

if [[ "$#" == 0 ]]; then
    echo "Usage: run.sh <path to Godot binary> [Godot command line options]"
	exit 1
fi

# The absolute path to the Godot binary
GODOT_BIN="$1"

# Additional command line options passed to Godot binary
GODOT_CMD="$2"

# Testing framework script path
TEST_FRAMEWORK="addons/gut/gut_cmdln.gd"

# Enviroment path (test project)
TEST_PATH="project"

# Change to the directory where the script is located,
# so it can be run from any location
cd "$(dirname "${BASH_SOURCE[0]}")"

# Run tests
if [[ "$GODOT_CMD" ]]; then
    "$GODOT_BIN" "$GODOT_CMD" --path "$TEST_PATH" -s "$TEST_FRAMEWORK"
else
    "$GODOT_BIN" --path "$TEST_PATH" -s "$TEST_FRAMEWORK"
fi
