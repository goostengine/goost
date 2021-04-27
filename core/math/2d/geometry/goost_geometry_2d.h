#ifndef GOOST_GEOMETRY_2D_H
#define GOOST_GEOMETRY_2D_H

#include "core/variant.h"

class GoostGeometry2D {
public:
	/* Polygon and polyline boolean operations */
	static Vector<Vector<Point2>> merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b);
	static Vector<Vector<Point2>> clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b);
	static Vector<Vector<Point2>> intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b);
	static Vector<Vector<Point2>> exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b);
	static Vector<Vector<Point2>> clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon);
	static Vector<Vector<Point2>> intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon);

	/* Polygon inflating and deflating */
	static Vector<Vector<Point2>> inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta);
	static Vector<Vector<Point2>> deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta);
	static Vector<Vector<Point2>> deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta);

	/* Polygon decomposition */
	static Vector<Vector<Point2>> triangulate_polygon(const Vector<Point2> &p_polygon);
	static Vector<Vector<Point2>> decompose_polygon(const Vector<Point2> &p_polygon);

	/* Polygon/Polyline attributes */
	static Point2 polygon_centroid(const Vector<Point2> &p_polygon);
	static real_t polygon_area(const Vector<Point2> &p_polygon);
	static real_t polygon_perimeter(const Vector<Point2> &p_polygon);
	static real_t polyline_length(const Vector<Point2> &p_polyline);
	static Rect2 bounding_rect(const Vector<Point2> &p_points);

	// Returns 0 if false, +1 if true, -1 if point is exactly on the polygon's boundary.
	static int point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon);

	/* Polygon/shapes generation methods */
	static Vector<Point2> regular_polygon(int p_edge_count, real_t p_size);
	static Vector<Point2> circle(real_t p_radius, real_t p_max_error = 0.25);
	
	/* Trigonometry utilities */
	static Vector2 to_vector(real_t p_angle);
};

#endif // GOOST_GEOMETRY_2D_H
