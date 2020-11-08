#include "goost_geometry_2d_bind.h"
#include "goost_geometry_2d.h"

_GoostGeometry2D *_GoostGeometry2D::singleton = nullptr;

Array _GoostGeometry2D::merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::merge_polygons(p_polygon_a, p_polygon_b);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::clip_polygons(p_polygon_a, p_polygon_b);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::intersect_polygons(p_polygon_a, p_polygon_b);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const {
	Vector<Vector<Point2>> polys = GoostGeometry2D::exclude_polygons(p_polygon_a, p_polygon_b);
	Array ret;
	for (int i = 0; i < polys.size(); ++i) {
		ret.push_back(polys[i]);
	}
	return ret;
}

Array _GoostGeometry2D::clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) const {
	Vector<Vector<Point2>> polylines = GoostGeometry2D::clip_polyline_with_polygon(p_polyline, p_polygon);
	Array ret;
	for (int i = 0; i < polylines.size(); ++i) {
		ret.push_back(polylines[i]);
	}
	return ret;
}

Array _GoostGeometry2D::intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) const {
	Vector<Vector<Point2>> polylines = GoostGeometry2D::intersect_polyline_with_polygon(p_polyline, p_polygon);
	Array ret;
	for (int i = 0; i < polylines.size(); ++i) {
		ret.push_back(polylines[i]);
	}
	return ret;
}

Array _GoostGeometry2D::inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::inflate_polygon(p_polygon, p_delta);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_polygon(p_polygon, p_delta);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::deflate_polyline(p_polyline, p_delta);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::triangulate_polygon(const Vector<Point2> &p_polygon) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::triangulate_polygon(p_polygon);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _GoostGeometry2D::decompose_polygon(const Vector<Point2> &p_polygon) const {
	Vector<Vector<Vector2>> solution = GoostGeometry2D::decompose_polygon(p_polygon);
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

void _GoostGeometry2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("merge_polygons", "polygon_a", "polygon_b"), &_GoostGeometry2D::merge_polygons);
	ClassDB::bind_method(D_METHOD("clip_polygons", "polygon_a", "polygon_b"), &_GoostGeometry2D::clip_polygons);
	ClassDB::bind_method(D_METHOD("intersect_polygons", "polygon_a", "polygon_b"), &_GoostGeometry2D::intersect_polygons);
	ClassDB::bind_method(D_METHOD("exclude_polygons", "polygon_a", "polygon_b"), &_GoostGeometry2D::exclude_polygons);
	ClassDB::bind_method(D_METHOD("clip_polyline_with_polygon", "polyline", "polygon"), &_GoostGeometry2D::clip_polyline_with_polygon);
	ClassDB::bind_method(D_METHOD("intersect_polyline_with_polygon", "polyline", "polygon"), &_GoostGeometry2D::intersect_polyline_with_polygon);

	ClassDB::bind_method(D_METHOD("inflate_polygon", "polygon", "delta"), &_GoostGeometry2D::inflate_polygon);
	ClassDB::bind_method(D_METHOD("deflate_polygon", "polygon", "delta"), &_GoostGeometry2D::deflate_polygon);
	ClassDB::bind_method(D_METHOD("deflate_polyline", "polyline", "delta"), &_GoostGeometry2D::deflate_polyline);

	ClassDB::bind_method(D_METHOD("triangulate_polygon", "polygon"), &_GoostGeometry2D::triangulate_polygon);
	ClassDB::bind_method(D_METHOD("decompose_polygon", "polygon"), &_GoostGeometry2D::decompose_polygon);

	ClassDB::bind_method(D_METHOD("polygon_centroid", "polygon"), &_GoostGeometry2D::polygon_centroid);
	ClassDB::bind_method(D_METHOD("polygon_area", "polygon"), &_GoostGeometry2D::polygon_area);
	ClassDB::bind_method(D_METHOD("polygon_perimeter", "polygon"), &_GoostGeometry2D::polygon_perimeter);
	ClassDB::bind_method(D_METHOD("polyline_length", "polyline"), &_GoostGeometry2D::polyline_length);

	ClassDB::bind_method(D_METHOD("point_in_polygon", "point", "polygon"), &_GoostGeometry2D::point_in_polygon);

	ClassDB::bind_method(D_METHOD("regular_polygon", "sides", "size"), &_GoostGeometry2D::regular_polygon);
	ClassDB::bind_method(D_METHOD("circle", "radius", "max_error"), &_GoostGeometry2D::circle, DEFVAL(0.25));
}

_GoostGeometry2D::_GoostGeometry2D() {
	singleton = this;
}
