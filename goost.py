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
    "core/math",
    "scene/physics",
    "scene/gui",
    "editor",
]

def get_components():
    comp = set()

    for n in components:
        parts = n.split("/")
        comp.update(parts)

    comp_list = list(comp)
    comp_list.sort()

    return comp_list


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


class GoostClass:
   def __init__(self, name, deps=[]):
      self.name = name
      self.deps = []

   def add_depencency(self, goost_class):
      self.deps.append(goost_class)

# A list of classes currently implemented in the extension, excluding any
# classes provided from within `modules/` directory.
# 
# This is used by `config.py::get_doc_classes()` and to disable each of the
# class via user-defined `custom.py::goost_classes_disabled` array of classes.
#
classes = [
    "GoostEngine",
    "GoostGeometry2D",
    "GoostImage",
    "GradientTexture2D",
    "GridRect",
    "ImageBlender",
    "ImageIndexed",
    "InvokeState",
    "LightTexture",
    "LinkedList",
    "ListNode",
    "PolyBoolean2D",
    "PolyBooleanParameters2D",
    "PolyDecomp2D",
    "PolyDecompParameters2D",
    "PolyOffset2D",
    "PolyOffsetParameters2D",
    "PolyNode2D",
    "PolyCircle2D",
    "PolyRectangle2D",
    "PolyShape2D",
    "PolyCollisionShape2D",
    "Random",
    "Random2D",
    "ShapeCast2D",
    "VariantMap",
    "VariantResource",
    "VisualShape2D",
]

# Convert to dictionary, because we need to instantiate `GoostClass` nodes.
_classes = {}
for c in classes:
    _classes[c] = GoostClass(c)
classes = _classes

# Class dependencies. If disabling classes above cause build errors,
# it's likely a dependency issue. If so, define them here explicitly.
#
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
classes["PolyCollisionShape2D"].add_depencency(classes["PolyNode2D"])

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

classes_enabled = []
classes_disabled = []
for c in classes:
    classes_enabled.append(c)
    classes_disabled.append(c)

import sys

custom_configured = False
try:
    import custom
    # Determine whether classes are enabled or disabled initially.
    classes_enabled_by_default = True
    if hasattr(custom, "classes_enabled_by_default"):
        classes_enabled_by_default = custom.classes_enabled_by_default

    if classes_enabled_by_default:
        classes_disabled.clear()
        for name, enabled in custom.classes.items():
            if not enabled:
                if not name in classes:
                    raise NameError("Goost: Requested to disable non-existing class `%s`" % name)
                classes_enabled.remove(name)
                classes_disabled.append(name)
    else:
        classes_enabled.clear()
        for name, enabled in custom.classes.items():
            if enabled:
                if not name in classes:
                    raise NameError("Goost: Requested to enable non-existing class `%s`" % name)
                classes_enabled.append(name)
                classes_disabled.remove(name)

    custom_configured = True

except ImportError:
    pass

except NameError as e:
    print(e)
    sys.exit(255)

if not custom_configured:
    # All classes are enabled by default.
    classes_disabled.clear()

# Check dependencies.
for c in classes_enabled:
    resolved = resolve_dependency(classes[c])
    for cr in resolved:
        if cr in classes_disabled:
            # Implicitly enable `cr` class because `c` depends on it.
            classes_enabled.append(cr)
            classes_disabled.remove(cr)
