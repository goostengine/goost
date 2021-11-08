import os

# This module is maintained as part of Goost, but can be built independently.
# Refer to https://github.com/goostengine/goost for more information.

def can_build(env, platform):
    libgit2_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "thirdparty/libgit2/")

    if os.path.isdir(libgit2_path):
        if not os.listdir(libgit2_path):
            return False
    else:
        return False

    build = env["tools"] and env["platform"] not in ["server", "javascript"]

    if "goost_vcs_enabled" in env:
        return build and env["goost_vcs_enabled"]

    return build


def configure(env):
	pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return ["EditorVCSInterfaceGit"]
