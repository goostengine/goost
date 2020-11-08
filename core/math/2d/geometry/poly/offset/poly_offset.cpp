#include "poly_offset.h"

PolyOffset2DBackend *PolyOffset2D::backend = nullptr;

void PolyOffset2DBackend::set_parameters(const Ref<PolyOffsetParameters2D> &p_parameters) {
	if (p_parameters.is_valid()) {
		parameters = p_parameters;
	} else {
		parameters = Ref<PolyOffsetParameters2D>();
		parameters = default_parameters;
		parameters->reset();
	}
}

Vector<Vector<Point2>> PolyOffset2D::inflate_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	backend->set_parameters(p_parameters);
	backend->get_parameters()->end_type = PolyOffsetParameters2D::END_POLYGON;
	return backend->offset_polypaths(p_polygons, -p_delta);
}

Vector<Vector<Point2>> PolyOffset2D::deflate_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	backend->set_parameters(p_parameters);
	backend->get_parameters()->end_type = PolyOffsetParameters2D::END_POLYGON;
	return backend->offset_polypaths(p_polygons, p_delta);
}

Vector<Vector<Point2>> PolyOffset2D::deflate_polylines(const Vector<Vector<Point2>> &p_polylines, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	backend->set_parameters(p_parameters);
	if (backend->get_parameters()->end_type == PolyOffsetParameters2D::END_POLYGON) {
		WARN_PRINT_ONCE("END_POLYGON does not apply for polyline deflating, fallback to END_JOINED.");
		backend->get_parameters()->end_type = PolyOffsetParameters2D::END_JOINED;
	}
	return backend->offset_polypaths(p_polylines, p_delta);
}

// BIND

_PolyOffset2D *_PolyOffset2D::singleton = nullptr;

Array _PolyOffset2D::inflate_polygons(Array p_polygons, real_t p_delta) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyOffsetParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyOffset2D::inflate_polygons(polygons, p_delta, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyOffset2D::deflate_polygons(Array p_polygons, real_t p_delta) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyOffsetParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyOffset2D::deflate_polygons(polygons, p_delta, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyOffset2D::deflate_polylines(Array p_polylines, real_t p_delta) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	const auto &params = singleton == this ? Ref<PolyOffsetParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyOffset2D::deflate_polylines(polylines, p_delta, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

void _PolyOffset2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &_PolyOffset2D::set_parameters);
	ClassDB::bind_method(D_METHOD("get_parameters"), &_PolyOffset2D::get_parameters);

	ClassDB::bind_method(D_METHOD("inflate_polygons", "polygons", "delta"), &_PolyOffset2D::inflate_polygons);
	ClassDB::bind_method(D_METHOD("deflate_polygons", "polygons", "delta"), &_PolyOffset2D::deflate_polygons);
	ClassDB::bind_method(D_METHOD("deflate_polylines", "polylines", "delta"), &_PolyOffset2D::deflate_polylines);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parameters"), "set_parameters", "get_parameters");
}

void PolyOffsetParameters2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_join_type", "join_type"), &PolyOffsetParameters2D::set_join_type);
	ClassDB::bind_method(D_METHOD("get_join_type"), &PolyOffsetParameters2D::get_join_type);
	ClassDB::bind_method(D_METHOD("set_end_type", "end_type"), &PolyOffsetParameters2D::set_end_type);
	ClassDB::bind_method(D_METHOD("get_end_type"), &PolyOffsetParameters2D::get_end_type);
	ClassDB::bind_method(D_METHOD("set_arc_tolerance", "arc_tolerance"), &PolyOffsetParameters2D::set_arc_tolerance);
	ClassDB::bind_method(D_METHOD("get_arc_tolerance"), &PolyOffsetParameters2D::get_arc_tolerance);
	ClassDB::bind_method(D_METHOD("set_miter_limit", "miter_limit"), &PolyOffsetParameters2D::set_miter_limit);
	ClassDB::bind_method(D_METHOD("get_miter_limit"), &PolyOffsetParameters2D::get_miter_limit);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "join_type"), "set_join_type", "get_join_type");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "end_type"), "set_end_type", "get_end_type");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "arc_tolerance"), "set_arc_tolerance", "get_arc_tolerance");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "miter_limit"), "set_miter_limit", "get_miter_limit");

	BIND_ENUM_CONSTANT(JOIN_SQUARE);
	BIND_ENUM_CONSTANT(JOIN_ROUND);
	BIND_ENUM_CONSTANT(JOIN_MITER);
	BIND_ENUM_CONSTANT(END_POLYGON);
	BIND_ENUM_CONSTANT(END_JOINED);
	BIND_ENUM_CONSTANT(END_BUTT);
	BIND_ENUM_CONSTANT(END_SQUARE);
	BIND_ENUM_CONSTANT(END_ROUND);
}
