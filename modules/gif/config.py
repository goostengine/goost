#!/usr/bin/env python3

def can_build(env, platform):
    # This module is part of the Goost project, but can be built independently.
    # Refer to https://github.com/goostengine/goost for more information.
    if "goost_image_enabled" in env:
        return env["goost_image_enabled"]
    return True


def configure(env):
    pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return ["ImageFrames"]
