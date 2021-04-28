# Complete list of functional components used by this extension.
#
# Components can be disabled with build options matching `goost_*_enabled=no`.
# A branch of components can be disabled as well, like: `goost_core_enabled=no`.
#
# NOTE: Components may not necessarily have structural meaning.
#
components = [
    "core/image",
    "core/math",
    "scene/physics",
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
#
# Complete list of all classes currently implemented in the extension,
# excluding any classes provided from within `modules/` directory.
# 
# This is used by config.py::get_doc_classes(), and potentially allow to disable
# each of the class in the future.
#
class GoostClass:
   def __init__(self, name, deps=[]):
      self.name = name
      self.deps = []

   def add_depencency(self, goost_class):
      self.deps.append(goost_class)

classes = [
    "GoostEngine",
    "GoostGeometry2D",
    "GoostImage",
    "GradientTexture2D",
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

# Define dependencies.
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
for c in classes:
    classes_enabled.append(c)

classes_disabled = []

try:
    import custom
    try:
        classes_disabled = custom.goost_classes_disabled
        for c in classes_disabled:
            if not c in classes:
                raise NameError("Goost: Requested to disable non-existing class.")
            classes_enabled.remove(c)
    except AttributeError:
        pass
except ImportError:
    pass
