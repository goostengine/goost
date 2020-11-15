#include "poly_shapes_2d.h"
#include "goost/core/math/2d/geometry/goost_geometry_2d.h"

void PolyCircle2D::set_radius(real_t p_radius) {
	ERR_FAIL_COND(p_radius <= 0);
	radius = p_radius;
	_queue_update();
	_change_notify("radius");
}

void PolyCircle2D::set_max_error(real_t p_max_error) {
	ERR_FAIL_COND(p_max_error <= 0);
	max_error = p_max_error;
	_queue_update();
	_change_notify("max_error");
}

Vector<Vector<Point2>> PolyCircle2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::circle(radius, max_error));
	return outlines;
}

void PolyCircle2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PolyCircle2D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &PolyCircle2D::get_radius);
	
	ClassDB::bind_method(D_METHOD("set_max_error", "max_error"), &PolyCircle2D::set_max_error);
	ClassDB::bind_method(D_METHOD("get_max_error"), &PolyCircle2D::get_max_error);
	
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_error", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,or_greater"), "set_max_error", "get_max_error");
}
