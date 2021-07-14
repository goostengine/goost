import os

def can_build(env, platform):
    libgit2_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "thirdparty/libgit2/src")
    if not os.path.exists(libgit2_path):
        return False

    # This module is part of the Goost project, but can be built independently.
    # Refer to https://github.com/goostengine/goost for more information.
    if "goost_vcs_enabled" in env:
        return env["goost_vcs_enabled"] and env["tools"]
    return env["tools"]

def configure(env):
	pass
