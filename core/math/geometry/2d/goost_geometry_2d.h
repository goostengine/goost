#pragma once

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

	/* Polygon/Polyline smoothing and simplification */
	static Vector<Point2> smooth_polygon(const Vector<Point2> &p_polygon, float p_density, float p_alpha = 0.5f);
	static Vector<Point2> smooth_polyline(const Vector<Point2> &p_polyline, float p_density, float p_alpha = 0.5f);
	static Vector<Point2> smooth_polygon_approx(const Vector<Point2> &p_polygon, int p_iterations = 1, float p_cut_distance = 0.25f);
	static Vector<Point2> smooth_polyline_approx(const Vector<Point2> &p_polyline, int p_iterations = 1, float p_cut_distance = 0.25f);
	static Vector<Point2> simplify_polyline(const Vector<Point2> &p_polyline, real_t p_epsilon);

	/* Polygon/Polyline attributes */
	static Point2 polygon_centroid(const Vector<Point2> &p_polygon);
	static real_t polygon_area(const Vector<Point2> &p_polygon);
	static real_t polygon_perimeter(const Vector<Point2> &p_polygon);
	static real_t polyline_length(const Vector<Point2> &p_polyline);
	static Rect2 bounding_rect(const Vector<Point2> &p_points);

	// Returns 0 if false, +1 if true, -1 if point is exactly on the polygon's boundary.
	static int point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon);

	/* Polygon/primitive generation methods */
	static Vector<Point2> rectangle(const Point2 &p_extents);
	static Vector<Point2> circle(real_t p_radius, real_t p_max_error = 0.25);
	static Vector<Point2> ellipse(real_t p_width, real_t p_height, real_t p_max_error = 0.25);
	static Vector<Point2> capsule(real_t p_radius, real_t p_height, real_t p_max_error = 0.25);
	static Vector<Point2> regular_polygon(int p_edge_count, real_t p_size);

	static Vector<Point2i> pixel_line(const Point2i &p_start, const Point2i &p_end);
	static Vector<Point2i> pixel_circle(int p_radius, const Point2i &p_origin = Point2i(0, 0));
	static Vector<Point2i> polyline_to_pixels(const Vector<Point2> &p_points);
	static Vector<Point2i> polygon_to_pixels(const Vector<Point2> &p_points);
};
