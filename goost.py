# General informational properties of the project, including version.
name = "Goost: Godot Engine Extension"
short_name = "goost"
url = "https://github.com/goostengine/goost"
website = "https://goostengine.github.io/"
version = {
    "major": 1,
    "minor": 1,
    "patch": 0,
    "status": "beta",
    "year": 2021,
}
# The following is a complete list of components this extension provides.
# Components can be disabled with build options matching `goost_*_enabled=no`.
# A branch of components can be disabled as well, like: `goost_core_enabled=no`.
#
# NOTE: Components may not necessarily have structural meaning.
#
components = [
    "core/image",
    "core/math/geometry",
    "scene/physics",
    "scene/gui",
    "editor",
]

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
# interest the via user-defined `custom.py::classes` dictionary.
#
# The key is the class name, and value is component that this class is part of.
# The list can contain classes provided by `modules/` directory, but they are
# only listed for documentation purposes here.
#
# Only rightmost child components are specified.
classes = {
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
    "PolyBoolean2D": "geometry",
    "PolyBooleanParameters2D": "geometry",
    "PolyDecomp2D": "geometry",
    "PolyDecompParameters2D": "geometry",
    "PolyOffset2D": "geometry",
    "PolyOffsetParameters2D": "geometry",
    "PolyNode2D": "geometry",
    "PolyCircle2D": "scene",
    "PolyRectangle2D": "scene",
    "PolyShape2D": "scene",
    "PolyCollisionShape2D": "physics",
    "Random": "math",
    "Random2D": "geometry",
    "ShapeCast2D": "physics",
    "VariantMap": "core",
    "VariantResource": "core",
    "VisualShape2D": "scene",
}

# Instantiate `GoostClass` nodes.
_classes = {}
for name in classes:
    _classes[name] = GoostClass(name)
    if not classes[name] in get_component_list():
        raise NameError("Component `%s` is not defined" % classes[name])
    _classes[name].component = classes[name]
classes = _classes

# Class dependencies. If disabling classes above cause build errors,
# it's likely a dependency issue. If so, define them here explicitly.

classes["GoostEngine"].add_depencency(classes["InvokeState"])

classes["GoostGeometry2D"].add_depencency(classes["PolyBoolean2D"])
classes["GoostGeometry2D"].add_depencency(classes["PolyDecomp2D"])
classes["GoostGeometry2D"].add_depencency(classes["PolyOffset2D"])

classes["LightTexture"].add_depencency(classes["GradientTexture2D"])
classes["LinkedList"].add_depencency(classes["ListNode"])

classes["PolyBoolean2D"].add_depencency(classes["PolyBooleanParameters2D"])
classes["PolyBoolean2D"].add_depencency(classes["PolyNode2D"])
classes["PolyDecomp2D"].add_depencency(classes["PolyDecompParameters2D"])
classes["PolyOffset2D"].add_depencency(classes["PolyOffsetParameters2D"])
classes["PolyCircle2D"].add_depencency(classes["PolyNode2D"])
classes["PolyRectangle2D"].add_depencency(classes["PolyNode2D"])
classes["PolyShape2D"].add_depencency(classes["PolyNode2D"])

classes["PolyCollisionShape2D"].add_depencency(classes["PolyShape2D"])
classes["PolyCollisionShape2D"].add_depencency(classes["PolyNode2D"])

classes["Random2D"].add_depencency(classes["GoostGeometry2D"])
classes["Random2D"].add_depencency(classes["Random"])


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


config_template = """# custom.py

components_enabled_by_default = True
components = {
    # "editor": False,
}

classes_enabled_by_default = True
classes = {
    # "LinkedList": False,
}
"""

if __name__ == "__main__":
    import os
    import sys
    import argparse
    import subprocess

    parser = argparse.ArgumentParser()

    parser.add_argument("--generate-config", action="store_true",
            help="Generates `custom.py` file to configure Goost components and classes.")

    parser.add_argument("--generate-doc-api", metavar="<path>",
            help="Generates a list of classes per component in `.rst` format.")

    args = parser.parse_args()

    if args.generate_config:
        # Generate custom.py.
        def write_config():
            with open("custom.py", "w") as f:
                f.write(config_template)

        if os.path.exists("custom.py"):
            print("Goost: The `custom.py` file already exists!")
            try:
                overwrite = input("Overwrite anyway? (y/N): ")
                if overwrite.lower() == "y":
                    write_config()
            except KeyboardInterrupt:
                print("n")
        else:
            write_config()

    if args.generate_doc_api:
        output_path = args.generate_doc_api
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
            "godot/doc/tools/makerst.py",
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
                f.write("    :caption: %s\n" % component.capitalize())
                f.write("    :name: toc-component-%s\n" % component)
                f.write("\n")
                for class_name in class_list:
                    f.write("    class_%s.rst\n" % class_name.lower())
                f.write("\n")
                parents = get_parent_components(component)
                parents.reverse()
                if parents:
                    f.write("**%s** is part of: " % component.capitalize())
                    for i in range(len(parents)):
                        f.write(":ref:`toc-component-%s`" % parents[i])
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
                f.write("    # Disable %s component.\n" % component)
                f.write("    scons goost_%s_enabled=no\n" % component)
                f.write("\n")
                f.write("    # Enable %s component, disable all others.\n" % component)
                f.write("    scons goost_components_enabled=no goost_%s_enabled=yes\n" % component)
                f.write("\n")

        print("Done. You can find generated files at `%s`" % os.path.abspath(output_path))
