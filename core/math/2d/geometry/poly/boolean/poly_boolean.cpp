#include "poly_boolean.h"

void PolyBooleanParameters2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_subject_fill_rule", "subject_fill_rule"), &PolyBooleanParameters2D::set_subject_fill_rule);
    ClassDB::bind_method(D_METHOD("get_subject_fill_rule"), &PolyBooleanParameters2D::get_subject_fill_rule);
    ClassDB::bind_method(D_METHOD("set_clip_fill_rule", "clip_fill_rule"), &PolyBooleanParameters2D::set_clip_fill_rule);
    ClassDB::bind_method(D_METHOD("get_clip_fill_rule"), &PolyBooleanParameters2D::get_clip_fill_rule);
    ClassDB::bind_method(D_METHOD("set_subject_open", "subject_open"), &PolyBooleanParameters2D::set_subject_open);
    ClassDB::bind_method(D_METHOD("is_subject_open"), &PolyBooleanParameters2D::is_subject_open);
    ClassDB::bind_method(D_METHOD("set_reverse_solution", "reverse_solution"), &PolyBooleanParameters2D::set_reverse_solution);
    ClassDB::bind_method(D_METHOD("is_reverse_solution"), &PolyBooleanParameters2D::is_reverse_solution);
    ClassDB::bind_method(D_METHOD("set_strictly_simple", "strictly_simple"), &PolyBooleanParameters2D::set_strictly_simple);
    ClassDB::bind_method(D_METHOD("is_strictly_simple"), &PolyBooleanParameters2D::is_strictly_simple);
    ClassDB::bind_method(D_METHOD("set_preserve_collinear", "preserve_collinear"), &PolyBooleanParameters2D::set_preserve_collinear);
    ClassDB::bind_method(D_METHOD("is_preserve_collinear"), &PolyBooleanParameters2D::is_preserve_collinear);
    
    BIND_ENUM_CONSTANT(FILL_RULE_EVEN_ODD);
    BIND_ENUM_CONSTANT(FILL_RULE_NON_ZERO);
    BIND_ENUM_CONSTANT(FILL_RULE_POSITIVE);
    BIND_ENUM_CONSTANT(FILL_RULE_NEGATIVE);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "subject_fill_rule"), "set_subject_fill_rule", "get_subject_fill_rule");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "clip_fill_rule"), "set_clip_fill_rule", "get_clip_fill_rule");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "subject_open"), "set_subject_open", "is_subject_open");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reverse_solution"), "set_reverse_solution", "is_reverse_solution");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "strictly_simple"), "set_strictly_simple", "is_strictly_simple");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "preserve_collinear"), "set_preserve_collinear", "is_preserve_collinear");
}

Ref<PolyNode2D> PolyNode2D::new_child(const Vector<Point2> &p_path) {
    Ref<PolyNode2D> child;
    child.instance();
    child->path = p_path;
    children.push_back(child);
    return child;
}

Ref<PolyNode2D> PolyNode2D::get_child(int p_idx) {
    ERR_FAIL_INDEX_V(p_idx, children.size(), nullptr);
    return children[p_idx];
}

Array PolyNode2D::get_children() const {
    Array ret;
    for (int i = 0; i < children.size(); ++i) {
        ret.push_back(children[i]);
    }
    return ret;
}

bool PolyNode2D::is_hole() const {
    bool hole = true;
    Ref<PolyNode2D> n = parent;
    while (n.is_valid()) {
      hole = !hole;
      n = n->parent;
    }
    return hole;
}

void PolyNode2D::clear() {
    children.clear();
}

void PolyNode2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_child", "path"), &PolyNode2D::new_child);
    ClassDB::bind_method(D_METHOD("get_child", "index"), &PolyNode2D::get_child);
    ClassDB::bind_method(D_METHOD("get_children"), &PolyNode2D::get_children);
    ClassDB::bind_method(D_METHOD("get_child_count"), &PolyNode2D::get_child_count);
    ClassDB::bind_method(D_METHOD("get_parent"), &PolyNode2D::get_parent);
    ClassDB::bind_method(D_METHOD("set_path", "path"), &PolyNode2D::set_path);
    ClassDB::bind_method(D_METHOD("get_path"), &PolyNode2D::get_path);
    ClassDB::bind_method(D_METHOD("is_hole"), &PolyNode2D::is_hole);
    ClassDB::bind_method(D_METHOD("clear"), &PolyNode2D::clear);
    
    ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "path"), "set_path", "get_path");
}
