import os
import goost

def make_classes_enabled(target, source, env):
    h = open(target[0].abspath, "w")

    h.write("// THIS FILE IS GENERATED, DO NOT EDIT!\n\n")
    h.write("#pragma once\n\n")
    for c in goost.classes_enabled:
        h.write("#define GOOST_%s\n" % c)
    h.write("\n")
    h.write("namespace goost {\n")
    for c in goost.classes_disabled:
        h.write("template <> void register_class<%s>();\n" % c)
    h.write("}\n")
    h.close()

    # NOTE: it's not required to generate this file if there are no classes
    # which can be disabled, but generating it prevents the file from being
    # rebuilt again by SCons, even if no changes were done to "goost.py".
    cpp = open(target[1].abspath, "w")
    cpp.write("// THIS FILE IS GENERATED, DO NOT EDIT!\n\n")
    cpp.write("#include \"register_types.h\"\n")
    cpp.write("#include \"classes_enabled.gen.h\"\n")
    cpp.write("\n")
    cpp.write("namespace goost {\n")
    for c in goost.classes_disabled:
        cpp.write("template <> void register_class<%s>() {}\n" % c)
    cpp.write("}\n")
    cpp.close()


def make_version_header(target, source, env):
    f = open(target[0].abspath, "w")
    f.write("// THIS FILE IS GENERATED, DO NOT EDIT!\n\n")

    f.write("#define GOOST_VERSION_MAJOR " + str(goost.version["major"]) + "\n")
    f.write("#define GOOST_VERSION_MINOR " + str(goost.version["minor"]) + "\n")
    f.write("#define GOOST_VERSION_PATCH " + str(goost.version["patch"]) + "\n")

    githash = ""
    gitfolder = ".git"

    if os.path.isfile(".git"):
        module_folder = open(".git", "r").readline().strip()
        if module_folder.startswith("gitdir: "):
            gitfolder = module_folder[8:]

    if os.path.isfile(os.path.join(gitfolder, "HEAD")):
        head = open(os.path.join(gitfolder, "HEAD"), "r").readline().strip()
        if head.startswith("ref: "):
            head = os.path.join(gitfolder, head[5:])
            if os.path.isfile(head):
                githash = open(head, "r").readline().strip()
        else:
            githash = head

    f.write('#define GOOST_VERSION_HASH "' + githash + '"' + "\n")
    f.write('#define GOOST_VERSION_STATUS "' + str(goost.version["status"]) + '"\n')
    f.write("#define GOOST_VERSION_YEAR " + str(goost.version["year"]) + "\n")
    
    f.close()
