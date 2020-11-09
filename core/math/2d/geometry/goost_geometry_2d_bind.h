#ifndef GOOST_GEOMETRY_2D_BIND_H
#define GOOST_GEOMETRY_2D_BIND_H

#include "core/object.h"

class _GoostGeometry2D : public Object {
	GDCLASS(_GoostGeometry2D, Object);

private:
	static _GoostGeometry2D *singleton;

protected:
	static void _bind_methods();

public:
	static _GoostGeometry2D *get_singleton() { return singleton; }

public:
	Array merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const;
	Array clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const;
	Array intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const;
	Array exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) const;
	Array clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) const;
	Array intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) const;

	Array inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) const;
	Array deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) const;
	Array deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta) const;

	Array triangulate_polygon(const Vector<Point2> &p_polygon) const;
	Array decompose_polygon(const Vector<Point2> &p_polygon) const;

	Vector2 polygon_centroid(const Vector<Vector2> &p_polygon) const;
	real_t polygon_area(const Vector<Vector2> &p_polygon) const;
	real_t polygon_perimeter(const Vector<Vector2> &p_polygon) const;
	real_t polyline_length(const Vector<Vector2> &p_polyline) const;

	int point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon) const;

	Vector<Point2> regular_polygon(int p_edge_count, real_t p_size) const;
	Vector<Point2> circle(real_t p_radius, real_t p_max_error) const;

	_GoostGeometry2D();
};

#endif // GOOST_GEOMETRY_2D_BIND_H
