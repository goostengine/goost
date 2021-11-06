# General informational properties of the project, including version.
name = "Goost: Godot Engine Extension"
short_name = "goost"
version = {
    "major": 1,
    "minor": 1,
    "patch": 0,
    "branch": "gd3",
    "status": "stable",
    "year": 2021,
}
url = "https://github.com/goostengine/goost"
doc_url = "https://goost.readthedocs.io/en/%s/" % version["branch"]
website = "https://goostengine.github.io/"

# The following is a complete list of components this extension provides.
# Components can be disabled with build options matching `goost_*_enabled=no`.
# A branch of components can be disabled as well, like: `goost_core_enabled=no`.
#
# Components may not necessarily have structural meaning.
components = [
    "core/script",
    "core/image",
    "core/math/geometry",
    "scene/physics",
    "scene/gui",
    "editor/vcs",
]

def get_component_readable_name(component):
    name = {
        "script": "Scripting",
        "image": "Image Processing",
        "geometry": "Geometry",
        "physics": "Physics",
        "gui": "User Interface",
        "vcs": "Version Control"
    }.get(component, "")
    if not name:
        name = component.capitalize()
    return name

def get_components(config={}, enabled_by_default=True):
    import sys

    component_list = get_component_list()

    if not config:
        for name in component_list:
            config[name] = enabled_by_default

    components_enabled = []
    components_enabled = component_list.copy()
    components_disabled = []
    components_disabled = component_list.copy()

    try:
        if enabled_by_default:
            components_disabled.clear()
            for name, enabled in config.items():
                if not enabled:
                    if not name in component_list:
                        raise NameError("Goost: Requested to disable non-existing component `%s`" % name)
                    components_enabled.remove(name)
                    components_disabled.append(name)
        else:
            components_enabled.clear()
            for name, enabled in config.items():
                if enabled:
                    if not name in component_list:
                        raise NameError("Goost: Requested to enable non-existing component `%s`" % name)
                    components_enabled.append(name)
                    components_disabled.remove(name)
    except NameError as e:
        print(e)
        sys.exit(255)

    ret = {
        "enabled": components_enabled,
        "disabled": components_disabled,
    }
    return ret


def get_component_list():
    component_set = set()

    for name in components:
        parts = name.split("/")
        component_set.update(parts)

    component_list = list(component_set)
    component_list.sort()

    return component_list


def get_child_components(parent):
    comp_list = []

    for n in components:
        parts = n.split("/")
        if not parent in parts:
            continue
        parts.reverse()
        for p in parts:
            if p == parent:
                break
            comp_list.append(p)

    return comp_list


def get_parent_components(child):
    comp_list = []

    for n in components:
        parts = n.split("/")
        if not child in parts:
            continue
        for p in parts:
            if p == child:
                break
            comp_list.append(p)

    return comp_list


class GoostClass:
    def __init__(self, name):
        self.name = name
        self.deps = []
        self.component = ""

    def add_depencency(self, goost_class):
        self.deps.append(goost_class)

# Classes currently implemented in the extension.
# 
# This is used by `config.py::get_doc_classes()` and to configure each class of
# interest via user-defined `custom.py::classes` dictionary.
#
# Key is the class name, and value is the component that this class is part of.
# The list can contain classes provided by `modules/` directory, but they are
# only listed for documentation purposes here.
#
# Only rightmost child components are specified.
classes = {
    "EditorVCSInterfaceGit": "vcs",  # modules/git
	"CommandLineHelpFormat": "core",
    "CommandLineOption": "core",
	"CommandLineParser": "core",
    "GoostEngine": "core",
    "GoostGeometry2D": "geometry",
    "GoostImage": "image",
    "GradientTexture2D": "scene",
    "GridRect": "gui",
    "ImageBlender": "image",
    "ImageFrames": "image",  # modules/gif
    "ImageIndexed": "image",
    "InvokeState": "core",
    "LightTexture": "scene",
    "LinkedList": "core",
    "ListNode": "core",
    "MixinScript": "script",
    "Mixin": "script",
    "PolyBoolean2D": "geometry",
    "PolyBooleanParameters2D": "geometry",
    "PolyDecomp2D": "geometry",
    "PolyDecompParameters2D": "geometry",
    "PolyOffset2D": "geometry",
    "PolyOffsetParameters2D": "geometry",
    "PolyCapsule2D": "scene",
    "PolyCircle2D": "scene",
    "PolyCollisionShape2D": "physics",
    "PolyNode2D": "geometry",
    "PolyPath2D": "geometry",
    "PolyRectangle2D": "scene",
    "PolyShape2D": "scene",
    "Random": "math",
    "Random2D": "geometry",
    "ShapeCast2D": "physics",
    "Stopwatch": "scene",
    "VariantMap": "core",
    "VariantResource": "core",
    "VisualShape2D": "scene",
}

# This is a list of all classes registered from within `modules/` directory.
# These cannot be disabled via `custom.py` configuration file.
module_classes = [
    "EditorVCSInterfaceGit",
    "GDScriptTranspiler",
    "ImageFrames",
]

# Instantiate `GoostClass` nodes.
_classes = {}
for name in classes:
    _classes[name] = GoostClass(name)
    if not classes[name] in get_component_list():
        raise NameError("Component `%s` is not defined" % classes[name])
    _classes[name].component = classes[name]
classes = _classes

# Class dependencies. If disabling classes above using `custom.py` cause
# compile/link errors or failing unit tests, it's likely a dependency issue.
# If so, define them here explicitly so that they're automatically enabled.
class_dependencies = {
    "CommandLineParser": ["CommandLineOption", "CommandLineHelpFormat"],
    "GoostEngine" : "InvokeState",
    "GoostGeometry2D" : ["PolyBoolean2D", "PolyDecomp2D", "PolyOffset2D"],
    "LightTexture" : "GradientTexture2D",
    "LinkedList" : "ListNode",
    "MixinScript" : "Mixin",
    "PolyBoolean2D" : ["PolyBooleanParameters2D", "PolyNode2D"],
    "PolyDecomp2D" : "PolyDecompParameters2D",
    "PolyCapsule2D" : ["GoostGeometry2D", "PolyNode2D"],
    "PolyCircle2D" : ["GoostGeometry2D", "PolyNode2D"],
    "PolyOffset2D" : "PolyOffsetParameters2D",
    "PolyPath2D" : ["PolyOffset2D", "PolyOffsetParameters2D"],
    "PolyRectangle2D" : "PolyNode2D",
    "PolyShape2D" : "PolyNode2D",
    "PolyCollisionShape2D" : ["PolyShape2D", "PolyNode2D"],
    "Random2D" : ["Random", "GoostGeometry2D"],
}
for name, dependencies in class_dependencies.items():
    if isinstance(dependencies, str):
        dependencies = [dependencies]
    for d in dependencies:
        classes[name].add_depencency(classes[d])


def resolve_dependency(goost_class):
    resolved = set()
    def resolve(c, r_resolved):
        for n in c.deps:
            resolve(n, r_resolved)
        r_resolved.add(c)
    resolve(goost_class, resolved)
    resolved_list = []
    for c in resolved:
        resolved_list.append(c.name)
    return resolved_list


def get_classes(config={}, enabled_by_default=True):
    import sys

    if not config:
        for c in classes:
            config[c] = enabled_by_default

    classes_enabled = []
    classes_disabled = []
    for c in classes:
        classes_enabled.append(c)
        classes_disabled.append(c)
    try:
        if enabled_by_default:
            classes_disabled.clear()
            for name, enabled in config.items():
                if not enabled:
                    if not name in classes:
                        raise NameError("Goost: Requested to disable non-existing class `%s`" % name)
                    classes_enabled.remove(name)
                    classes_disabled.append(name)
        else:
            classes_enabled.clear()
            for name, enabled in config.items():
                if enabled:
                    if not name in classes:
                        raise NameError("Goost: Requested to enable non-existing class `%s`" % name)
                    classes_enabled.append(name)
                    classes_disabled.remove(name)
    except NameError as e:
        print(e)
        sys.exit(255)

    # Check dependencies.
    for c in classes_enabled:
        resolved = resolve_dependency(classes[c])
        for cr in resolved:
            if cr in classes_disabled:
                # Implicitly enable `cr` class because `c` depends on it.
                classes_enabled.append(cr)
                classes_disabled.remove(cr)
    ret = {
        "enabled": classes_enabled,
        "disabled": classes_disabled,
    }
    return ret


def get_class_components(name):
    components = get_parent_components(classes[name].component)
    components.append(classes[name].component)
    return components


def get_component_classes(component):
    class_list = []
    for n, c in classes.items():
        if c.component == component:
            class_list.append(n)
    return class_list


if __name__ == "__main__":
    import os
    import sys
    import argparse
    import subprocess

    parser = argparse.ArgumentParser(prog="goost")
    sub = parser.add_subparsers(dest="tool")

    parser_config = sub.add_parser("config",
            help="Configure Goost components and classes.")
    config_action = parser_config.add_mutually_exclusive_group()

    config_action.add_argument("--enable", action="store_true",
            help="Enable all components and classes by default.")

    config_action.add_argument("--disable", action="store_true",
            help="Disable all components and classes by default.")

    config_action.add_argument("--update", action="store_true",
            help="Update existing configuration (default).")

    parser_doc = sub.add_parser("doc", help="Generate Goost API documentation.")
    parser_doc.add_argument("--generate-api", metavar="<path>", required=True,
            help="Generates a list of classes per component in `.rst` format.")

    args = parser.parse_args()

    if args.tool == "config":
        # Generate or update custom.py.
        must_overwrite_config = args.enable or args.disable
        must_update_config = not must_overwrite_config

        enable_by_default = True
        if args.enable:
            enable_by_default = True
        elif args.disable:
            enable_by_default = False

        custom_exists = os.path.exists("custom.py")

        def write_config():
            scons_options = {} # The ones defined in SConstruct.
            components_config = {}
            components_enabled_by_default = enable_by_default
            classes_config = {}
            classes_enabled_by_default = enable_by_default

            if must_update_config:
                try:
                    import custom
                    custom_attributes = [item for item in dir(custom) if not item.startswith("__")]
                    for attr in custom_attributes:
                        if attr in ["components", "components_enabled_by_default", "classes", "classes_enabled_by_default"]:
                            continue
                        scons_options[attr] = getattr(custom, attr)

                    if hasattr(custom, "components"):
                        for name, enabled in custom.components.items():
                            if not name in get_component_list():
                                print("Goost: Removing non-existing component: %s" % name)
                                continue
                            components_config[name] = enabled

                    if hasattr(custom, "components_enabled_by_default"):
                        components_enabled_by_default = custom.components_enabled_by_default

                    if hasattr(custom, "classes"):
                        for name, enabled in custom.classes.items():
                            if not name in classes:
                                print("Goost: Removing non-existing class: %s" % name)
                                continue
                            classes_config[name] = enabled

                    if hasattr(custom, "classes_enabled_by_default"):
                        classes_enabled_by_default = custom.classes_enabled_by_default

                except ImportError:
                    pass # Does not exist yet.
                except SyntaxError as e:
                    print("Goost: " + str(e))
                    def skip_and_exit():
                        print("Goost: Skipping configuration.")
                        sys.exit(255)
                    try:
                        answer = input("Would you like to overwrite `custom.py`? (y/N): ")
                        if not answer or answer.lower() == "n":
                            skip_and_exit()
                    except KeyboardInterrupt:
                        print("n")
                        skip_and_exit()

            update_count = 0

            for name in get_component_list():
                if name in components_config:
                    continue
                if must_update_config or not custom_exists:
                    print("Goost: Adding new component: `%s`" % name)
                update_count += 1
                components_config[name] = components_enabled_by_default

            for name in classes:
                if name in classes_config:
                    continue
                if name in module_classes:
                    # Modules are self-contained just like Goost,
                    # we cannot enable/disable individual classes from there.
                    continue
                if must_update_config or not custom_exists:
                    print("Goost: Adding new class: `%s`" % name)
                update_count += 1
                classes_config[name] = classes_enabled_by_default

            with open("custom.py", "w") as f:
                f.write("# custom.py\n")
                for name, value in sorted(scons_options.items()):
                    f.write('%s = "%s"\n' % (name, value))
                f.write("\n")

                f.write("components_enabled_by_default = %s\n" % components_enabled_by_default)
                f.write("components = {\n")
                components_max_length = len(max(components_config.keys(), key=len))
                for name, enabled in sorted(components_config.items()):
                    # Write aligned, makes it easier to edit.
                    f.write('    %-0*s %s,\n' % (components_max_length + 3, '"%s":' %  name, enabled))
                f.write("}\n")
                f.write("\n")

                f.write("classes_enabled_by_default = %s\n" % classes_enabled_by_default)
                f.write("classes = {\n")
                classes_max_length = len(max(classes_config.keys(), key=len))
                for name, enabled in sorted(classes_config.items()):
                    class_components = get_class_components(name)
                    class_components.reverse()
                    comps = ""
                    for i in range(len(class_components)):
                        comps += class_components[i]
                        if i < len(class_components) - 1:
                            comps += " > "
                    # Write aligned, makes it easier to edit.
                    f.write('    %-0*s %s,  # %s\n' % (classes_max_length + 3, '"%s":' %  name, enabled, comps))
                f.write("}\n")

                # Add links to each of the class in the Goost API documentation.
                def write_row_line(f):
                    f.write("#" + "=" * 119 + "\n")
                f.write("\n")
                write_row_line(f)
                f.write("# Goost API:\n")
                write_row_line(f)
                for name in classes_config:
                    f.write("# %-0*s" % (classes_max_length + 1, name) + doc_url + "classes/class_%s.html\n" % name.lower())
                write_row_line(f)

            return update_count

        if not custom_exists:
            print("Goost: Generating `custom.py` file ...")
        elif args.enable:
            print("Goost: Enabling all components and classes in `custom.py` by default ...")
        elif args.disable:
            print("Goost: Disabling all components and classes in `custom.py` by default ...")
        else:
            print("Goost: Updating `custom.py` file ...")

        update_count = write_config()
        if update_count == 0 and must_update_config:
            print("Goost: Already up-to-date.")

        if not custom_exists:
            print("Done generating Goost configuration file.")
            print("Edit `./custom.py` to customize components and classes to build.")
            print("You can run this command several times to update existing configuration.")
            print()
            if args.enable:
                print("All components/classes are enabled by default. If you'd like to disable")
                print("them all by default, run the following command:")
                print()
                print("    python goost.py config --disable")
                print()
            print("Once done, run `scons` to start building Godot with Goost.")
            print()
            print("If you'd like to know more, refer to official Goost documentation:")
            print("  - https://goost.readthedocs.io/en/gd3/")
            print()

    elif args.tool == "doc" and args.generate_api:
        output_path = args.generate_api
        if not os.path.exists(output_path):
            os.makedirs(output_path)

        def write_comment_warning(f):
            f.write(".. THIS FILE IS GENERATED, DO NOT EDIT!\n")
            f.write(".. Use `python goost.py --generate-doc-api` at Goost's source tree instead.\n\n")

        # Generate Goost class reference.
        if not os.path.exists("godot"):
            print("Error: Cannot find `godot` repository at Goost's source tree.")
            print("Please run `scons` command first.")
            sys.exit(255)

        subprocess.run([sys.executable,
            "godot/doc/tools/make_rst.py",
            "godot/doc/classes",
            "godot/modules",
            "doc",
            "modules",
            "--output", output_path,
            "--filter", "^(?!.*godot)",
        ])

        print("Generating Goost API per component... ")
        with open(os.path.join(output_path, "index.rst"), "w") as f:
            f.write(":github_url: hide\n")
            f.write("\n")
            write_comment_warning(f)
            f.write(".. _goost_api:\n")
            f.write("\n")
            f.write("Goost API\n")
            f.write("=========\n")
            f.write("\n")
            f.write("This is a list of all classes provided by Goost components.\n")
            f.write("\n")
            f.write("All components are enabled by default, unless overridden via command-line or\n"
                    "configuration file, please refer to :ref:`doc_configuring_the_build` page for\n"
                    "further instructions.\n")
            f.write("\n")
            for component in sorted(get_component_list()):
                class_list = sorted(get_component_classes(component))
                if not class_list:
                    continue
                f.write(".. toctree::\n")
                f.write("    :maxdepth: 1\n")
                caption = get_component_readable_name(component)
                f.write("    :caption: %s\n" % caption)
                f.write("    :name: toc-component-%s\n" % component)
                f.write("\n")
                for class_name in class_list:
                    f.write("    class_%s.rst\n" % class_name.lower())
                f.write("\n")
                parents = get_parent_components(component)
                parents.reverse()
                if parents:
                    f.write("**%s** is part of: " % caption)
                    for i in range(len(parents)):
                        if len(get_component_classes(parents[i])) > 0:
                            f.write(":ref:`toc-component-%s`" % parents[i])
                        else:
                            f.write(parents[i].capitalize())
                        if i < len(parents) - 1:
                            f.write(" **>** ")
                    f.write("\n\n")

        # User-facing component info to be included in `Components` sections.
        # This includes a list of classes per component and CLI usage examples.
        for component in sorted(get_component_list()):
            class_list = sorted(get_component_classes(component))
            # Using `rsti` extension to denote that it's not an actual document,
            # but an include file used in other docs (prevent toctree warnings).
            with open(os.path.join(output_path, "component_" + component + ".rsti"), "w") as f:
                write_comment_warning(f)
                f.write("Classes\n")
                f.write("-------\n")
                f.write("\n")
                for class_name in class_list:
                    f.write("* :ref:`%s<class_%s>`\n" % (class_name, class_name))
                f.write("\n")
                f.write("Usage\n")
                f.write("-----\n")
                f.write("\n")
                f.write(".. code-block:: shell\n")
                f.write("\n")
                f.write("    # Disable \"%s\" component.\n" % get_component_readable_name(component))
                f.write("    scons goost_%s_enabled=no\n" % component)
                f.write("\n")
                f.write("    # Enable \"%s\" component, disable all others.\n" % get_component_readable_name(component))
                f.write("    scons goost_components_enabled=no goost_%s_enabled=yes\n" % component)
                f.write("\n")

        print("Done. You can find generated files at `%s`" % os.path.abspath(output_path))
