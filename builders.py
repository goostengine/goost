import os
from compat import iteritems, itervalues, open_utf8, escape_string

import goost


def make_classes_enabled(target, source, env):
    h = open(target[0].abspath, "w")

    h.write("// THIS FILE IS GENERATED, DO NOT EDIT!\n\n")
    h.write("#pragma once\n\n")
    for c in env["goost_classes_enabled"]:
        h.write("#define GOOST_%s\n" % c)
    h.write("\n")
    h.write("namespace goost {\n")
    for c in env["goost_classes_disabled"]:
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
    for c in env["goost_classes_disabled"]:
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
    gitfolder = os.path.join(os.path.dirname(os.path.realpath(__file__)), ".git")

    if os.path.isfile(gitfolder):
        module_folder = open(gitfolder, "r").readline().strip()
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

# Implementation synchronized with upstream (change "GODOT" to "GOOST"):
# https://github.com/godotengine/godot/blob/master/core/core_builders.py
def make_authors_header(target, source, env):
    sections = ["Project Founder", "Lead Developer", "Project Manager", "Developers"]
    sections_id = [
        "AUTHORS_FOUNDERS",
        "AUTHORS_LEAD_DEVELOPERS",
        "AUTHORS_PROJECT_MANAGERS",
        "AUTHORS_DEVELOPERS",
    ]
    src = source[0].abspath
    dst = target[0].abspath
    f = open_utf8(src, "r")
    g = open_utf8(dst, "w")

    g.write("// THIS FILE IS GENERATED, DO NOT EDIT!\n")
    g.write("#ifndef GOOST_AUTHORS_H\n")
    g.write("#define GOOST_AUTHORS_H\n")

    reading = False

    def close_section():
        g.write("\t0\n")
        g.write("};\n")

    for line in f:
        if reading:
            if line.startswith("    "):
                g.write('\t"' + escape_string(line.strip()) + '",\n')
                continue
        if line.startswith("## "):
            if reading:
                close_section()
                reading = False
            for section, section_id in zip(sections, sections_id):
                if line.strip().endswith(section):
                    current_section = escape_string(section_id)
                    reading = True
                    g.write("const char *const " + current_section + "[] = {\n")
                    break
    if reading:
        close_section()

    g.write("#endif\n")

    g.close()
    f.close()

# Implementation synchronized with upstream (change "GODOT" to "GOOST"):
# https://github.com/godotengine/godot/blob/master/core/core_builders.py
def make_license_header(target, source, env):
    src_copyright = source[0].abspath
    src_license = source[1].abspath
    dst = target[0].abspath

    class LicenseReader:
        def __init__(self, license_file):
            self._license_file = license_file
            self.line_num = 0
            self.current = self.next_line()

        def next_line(self):
            line = self._license_file.readline()
            self.line_num += 1
            while line.startswith("#"):
                line = self._license_file.readline()
                self.line_num += 1
            self.current = line
            return line

        def next_tag(self):
            if not ":" in self.current:
                return ("", [])
            tag, line = self.current.split(":", 1)
            lines = [line.strip()]
            while self.next_line() and self.current.startswith(" "):
                lines.append(self.current.strip())
            return (tag, lines)

    from collections import OrderedDict

    projects = OrderedDict()
    license_list = []

    with open_utf8(src_copyright, "r") as copyright_file:
        reader = LicenseReader(copyright_file)
        part = {}
        while reader.current:
            tag, content = reader.next_tag()
            if tag in ("Files", "Copyright", "License"):
                part[tag] = content[:]
            elif tag == "Comment":
                # attach part to named project
                projects[content[0]] = projects.get(content[0], []) + [part]

            if not tag or not reader.current:
                # end of a paragraph start a new part
                if "License" in part and not "Files" in part:
                    # no Files tag in this one, so assume standalone license
                    license_list.append(part["License"])
                part = {}
                reader.next_line()

    data_list = []
    for project in itervalues(projects):
        for part in project:
            part["file_index"] = len(data_list)
            data_list += part["Files"]
            part["copyright_index"] = len(data_list)
            data_list += part["Copyright"]

    with open_utf8(dst, "w") as f:

        f.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        f.write("#ifndef GOOST_LICENSE_H\n")
        f.write("#define GOOST_LICENSE_H\n")
        f.write("const char *const GOOST_LICENSE_TEXT =")

        with open_utf8(src_license, "r") as license_file:
            for line in license_file:
                escaped_string = escape_string(line.strip())
                f.write('\n\t\t"' + escaped_string + '\\n"')
        f.write(";\n\n")

        f.write(
            "struct ComponentCopyrightPart {\n"
            "\tconst char *license;\n"
            "\tconst char *const *files;\n"
            "\tconst char *const *copyright_statements;\n"
            "\tint file_count;\n"
            "\tint copyright_count;\n"
            "};\n\n"
        )

        f.write(
            "struct ComponentCopyright {\n"
            "\tconst char *name;\n"
            "\tconst ComponentCopyrightPart *parts;\n"
            "\tint part_count;\n"
            "};\n\n"
        )

        f.write("const char *const COPYRIGHT_INFO_DATA[] = {\n")
        for line in data_list:
            f.write('\t"' + escape_string(line) + '",\n')
        f.write("};\n\n")

        f.write("const ComponentCopyrightPart COPYRIGHT_PROJECT_PARTS[] = {\n")
        part_index = 0
        part_indexes = {}
        for project_name, project in iteritems(projects):
            part_indexes[project_name] = part_index
            for part in project:
                f.write(
                    '\t{ "'
                    + escape_string(part["License"][0])
                    + '", '
                    + "&COPYRIGHT_INFO_DATA["
                    + str(part["file_index"])
                    + "], "
                    + "&COPYRIGHT_INFO_DATA["
                    + str(part["copyright_index"])
                    + "], "
                    + str(len(part["Files"]))
                    + ", "
                    + str(len(part["Copyright"]))
                    + " },\n"
                )
                part_index += 1
        f.write("};\n\n")

        f.write("const int COPYRIGHT_INFO_COUNT = " + str(len(projects)) + ";\n")

        f.write("const ComponentCopyright COPYRIGHT_INFO[] = {\n")
        for project_name, project in iteritems(projects):
            f.write(
                '\t{ "'
                + escape_string(project_name)
                + '", '
                + "&COPYRIGHT_PROJECT_PARTS["
                + str(part_indexes[project_name])
                + "], "
                + str(len(project))
                + " },\n"
            )
        f.write("};\n\n")

        f.write("const int LICENSE_COUNT = " + str(len(license_list)) + ";\n")

        f.write("const char *const LICENSE_NAMES[] = {\n")
        for l in license_list:
            f.write('\t"' + escape_string(l[0]) + '",\n')
        f.write("};\n\n")

        f.write("const char *const LICENSE_BODIES[] = {\n\n")
        for l in license_list:
            for line in l[1:]:
                if line == ".":
                    f.write('\t"\\n"\n')
                else:
                    f.write('\t"' + escape_string(line) + '\\n"\n')
            f.write('\t"",\n\n')
        f.write("};\n\n")

        f.write("#endif\n")
