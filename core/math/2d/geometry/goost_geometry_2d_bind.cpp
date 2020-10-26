#include "goost_geometry_2d_bind.h"

_GoostGeometry2D *_GoostGeometry2D::singleton = nullptr;

Array _GoostGeometry2D::merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::merge_polygons(p_polygon_a, p_polygon_b, p_params);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::clip_polygons(p_polygon_a, p_polygon_b, p_params);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::intersect_polygons(p_polygon_a, p_polygon_b, p_params);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::exclude_polygons(p_polygon_a, p_polygon_b, p_params);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::merge_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::merge_multiple_polygons(polygons_a, polygons_b, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::clip_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::clip_multiple_polygons(polygons_a, polygons_b, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::intersect_multiple_polygons(polygons_a, polygons_b, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::exclude_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::exclude_multiple_polygons(polygons_a, polygons_b, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::polygons_boolean(PolyBooleanOperation p_op, Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::polygons_boolean(GoostGeometry2D::PolyBooleanOperation(p_op), polygons_a, polygons_b, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Ref<PolyNode2D> _GoostGeometry2D::polygons_boolean_tree(PolyBooleanOperation p_op, Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); i++) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Point2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); i++) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	return GoostGeometry2D::polygons_boolean_tree(GoostGeometry2D::PolyBooleanOperation(p_op), polygons_a, polygons_b, p_params);
}

Array _GoostGeometry2D::clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polylines = GoostGeometry2D::clip_polyline_with_polygon(p_polyline, p_polygon, p_params);
	Array ret;
	for (int i = 0; i < polylines.size(); ++i) {
		ret.push_back(polylines[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polylines = GoostGeometry2D::intersect_polyline_with_polygon(p_polyline, p_polygon, p_params);
	Array ret;
	for (int i = 0; i < polylines.size(); ++i) {
		ret.push_back(polylines[i]);
	}
	return ret;
}

Array _GoostGeometry2D::clip_multiple_polylines_with_polygons(const Array &p_polylines, const Array &p_polygons, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::clip_multiple_polylines_with_polygons(polylines, polygons, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_multiple_polylines_with_polygons(const Array &p_polylines, const Array &p_polygons, Ref<PolyBooleanParameters2D> p_params) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::intersect_multiple_polylines_with_polygons(polylines, polygons, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::inflate_polygon(p_polygon, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_polygon(p_polygon, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::inflate_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::inflate_multiple_polygons(polygons, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_multiple_polygons(polygons, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_polyline(p_polyline, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_multiple_polylines(const Array &p_polylines, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_multiple_polylines(polylines, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::offset_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::offset_polygon(p_polygon, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::offset_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::offset_multiple_polygons(polygons, p_delta, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::triangulate_polygon(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::triangulate_polygon(p_polygon, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::decompose_polygon_into_convex(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::decompose_polygon_into_convex(p_polygon, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::triangulate_multiple_polygons(const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::triangulate_multiple_polygons(polygons, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::decompose_multiple_polygons_into_convex(const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::decompose_multiple_polygons_into_convex(polygons, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::decompose_polygons(PolyDecompType p_type, const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	Vector<Vector<Vector2>> solution = GoostGeometry2D::decompose_polygons(GoostGeometry2D::PolyDecompType(p_type), polygons, p_params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Vector2 _GoostGeometry2D::polygon_centroid(const Vector<Vector2> &p_polygon) const {
	return GoostGeometry2D::polygon_centroid(p_polygon);
}

real_t _GoostGeometry2D::polygon_area(const Vector<Vector2> &p_polygon) const {
	return GoostGeometry2D::polygon_area(p_polygon);
}

real_t _GoostGeometry2D::polygon_perimeter(const Vector<Vector2> &p_polygon) const {
	return GoostGeometry2D::polygon_perimeter(p_polygon);
}

real_t _GoostGeometry2D::polyline_length(const Vector<Vector2> &p_polyline) const {
	return GoostGeometry2D::polyline_length(p_polyline);
}

int _GoostGeometry2D::point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon) const {
	return GoostGeometry2D::point_in_polygon(p_point, p_polygon);
}

Vector<Point2> _GoostGeometry2D::regular_polygon(int p_edge_count, real_t p_size) const {
	return GoostGeometry2D::regular_polygon(p_edge_count, p_size);
}

Vector<Point2> _GoostGeometry2D::circle(real_t p_radius, real_t p_max_error) const {
	return GoostGeometry2D::circle(p_radius, p_max_error);
}

Vector2 _GoostGeometry2D::rand_point_in_circle(real_t p_radius) const {
	return GoostGeometry2D::rand_point_in_circle(p_radius);
}

Vector2 _GoostGeometry2D::rand_point_on_circle(real_t p_radius) const {
	return GoostGeometry2D::rand_point_on_circle(p_radius);
}

Vector2 _GoostGeometry2D::rand_point_in_ring(real_t p_min_radius, real_t p_max_radius) const {
	return GoostGeometry2D::rand_point_in_ring(p_min_radius, p_max_radius);
}

void _GoostGeometry2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("merge_polygons", "polygon_a", "polygon_b", "params"), &_GoostGeometry2D::merge_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clip_polygons", "polygon_a", "polygon_b", "params"), &_GoostGeometry2D::clip_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("intersect_polygons", "polygon_a", "polygon_b", "params"), &_GoostGeometry2D::intersect_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("exclude_polygons", "polygon_a", "polygon_b", "params"), &_GoostGeometry2D::exclude_polygons, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("merge_multiple_polygons", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::merge_multiple_polygons, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clip_multiple_polygons", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::clip_multiple_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("intersect_multiple_polygons", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::intersect_multiple_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("exclude_multiple_polygons", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::exclude_multiple_polygons, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("polygons_boolean", "operation", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::polygons_boolean, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("polygons_boolean_tree", "operation", "polygons_a", "polygons_b", "params"), &_GoostGeometry2D::polygons_boolean_tree, DEFVAL(Variant()), DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("clip_polyline_with_polygon", "polyline", "polygon", "params"), &_GoostGeometry2D::clip_polyline_with_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("intersect_polyline_with_polygon", "polyline", "polygon", "params"), &_GoostGeometry2D::intersect_polyline_with_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clip_multiple_polylines_with_polygons", "polylines", "polygons", "params"), &_GoostGeometry2D::clip_multiple_polylines_with_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("intersect_multiple_polylines_with_polygons", "polylines", "polygons", "params"), &_GoostGeometry2D::intersect_multiple_polylines_with_polygons, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("inflate_polygon", "polygon", "delta", "params"), &_GoostGeometry2D::inflate_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("deflate_polygon", "polygon", "delta", "params"), &_GoostGeometry2D::deflate_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("inflate_multiple_polygons", "polygons", "delta", "params"), &_GoostGeometry2D::inflate_multiple_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("deflate_multiple_polygons", "polygons", "delta", "params"), &_GoostGeometry2D::deflate_multiple_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("deflate_polyline", "polyline", "delta", "params"), &_GoostGeometry2D::deflate_polyline, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("deflate_multiple_polylines", "polylines", "delta", "params"), &_GoostGeometry2D::deflate_multiple_polylines, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("offset_polygon", "polygon", "delta", "params"), &_GoostGeometry2D::offset_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("offset_multiple_polygons", "polygons", "delta", "params"), &_GoostGeometry2D::offset_multiple_polygons, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("triangulate_polygon", "polygon", "params"), &_GoostGeometry2D::triangulate_polygon, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("triangulate_multiple_polygons", "polygon", "params"), &_GoostGeometry2D::triangulate_multiple_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("decompose_polygon_into_convex", "polygon", "params"), &_GoostGeometry2D::decompose_polygon_into_convex, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("decompose_multiple_polygons_into_convex", "polygon", "params"), &_GoostGeometry2D::decompose_multiple_polygons_into_convex, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("decompose_polygons", "type", "polygons", "params"), &_GoostGeometry2D::decompose_polygons, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("polygon_centroid", "polygon"), &_GoostGeometry2D::polygon_centroid);
	ClassDB::bind_method(D_METHOD("polygon_area", "polygon"), &_GoostGeometry2D::polygon_area);
	ClassDB::bind_method(D_METHOD("polygon_perimeter", "polygon"), &_GoostGeometry2D::polygon_perimeter);
	ClassDB::bind_method(D_METHOD("polyline_length", "polyline"), &_GoostGeometry2D::polyline_length);

	ClassDB::bind_method(D_METHOD("point_in_polygon", "point", "polygon"), &_GoostGeometry2D::point_in_polygon);

	ClassDB::bind_method(D_METHOD("regular_polygon", "sides", "size"), &_GoostGeometry2D::regular_polygon, DEFVAL(64.0));
	ClassDB::bind_method(D_METHOD("circle", "radius", "max_error"), &_GoostGeometry2D::circle, DEFVAL(0.25));

	ClassDB::bind_method(D_METHOD("rand_point_in_circle", "radius"), &_GoostGeometry2D::rand_point_in_circle);
	ClassDB::bind_method(D_METHOD("rand_point_on_circle", "radius"), &_GoostGeometry2D::rand_point_on_circle);
	ClassDB::bind_method(D_METHOD("rand_point_in_ring", "min_radius", "max_radius"), &_GoostGeometry2D::rand_point_in_ring);

	BIND_ENUM_CONSTANT(OPERATION_NONE);
	BIND_ENUM_CONSTANT(OPERATION_UNION);
	BIND_ENUM_CONSTANT(OPERATION_DIFFERENCE);
	BIND_ENUM_CONSTANT(OPERATION_INTERSECTION);
	BIND_ENUM_CONSTANT(OPERATION_XOR);

	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_EC);
	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_OPT);
	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_MONO);
	BIND_ENUM_CONSTANT(DECOMP_CONVEX_HM);
	BIND_ENUM_CONSTANT(DECOMP_CONVEX_OPT);
}

_GoostGeometry2D::_GoostGeometry2D() {
	singleton = this;
}
