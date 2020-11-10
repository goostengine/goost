#include "poly_node_2d.h"

PolyNode2D *PolyNode2D::new_child(const Vector<Point2> &p_points) {
	PolyNode2D *child = memnew(PolyNode2D);
	child->points = p_points;
	add_child(child);
	return child;
}

bool PolyNode2D::is_hole() const {
	bool hole = true;
	PolyNode2D *n = Object::cast_to<PolyNode2D>(get_parent());
	while (n) {
		hole = !hole;
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
	return hole;
}

void PolyNode2D::clear() {
	for (int i = 0; i < get_child_count(); ++i) {
        get_child(i)->queue_delete();
    }
}

void PolyNode2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_points", "points"), &PolyNode2D::set_points);
	ClassDB::bind_method(D_METHOD("get_points"), &PolyNode2D::get_points);

	ClassDB::bind_method(D_METHOD("new_child", "from_points"), &PolyNode2D::new_child);
	ClassDB::bind_method(D_METHOD("is_hole"), &PolyNode2D::is_hole);
	ClassDB::bind_method(D_METHOD("clear"), &PolyNode2D::clear);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "points"), "set_points", "get_points");
}
