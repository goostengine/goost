import os

# This module is part of the Goost project, but can be built independently.
# Refer to https://github.com/goostengine/goost for more information.
def can_build(env, platform):
    libgit2_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "thirdparty/libgit2/src")
    if not os.path.exists(libgit2_path):
        return False

    build = env["tools"] and env["platform"] != "server"

    if "goost_vcs_enabled" in env:
        return build and env["goost_vcs_enabled"]

    return build


def configure(env):
	pass


def get_doc_path():
    return "doc"


def get_doc_classes():
    return ["EditorVCSInterfaceGit"]
