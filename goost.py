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
