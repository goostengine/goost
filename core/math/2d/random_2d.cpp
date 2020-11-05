#include "random_2d.h"
#include "goost/core/math/2d/geometry/goost_geometry_2d.h"

#include "core/method_bind_ext.gen.inc"

Random2D *Random2D::singleton = nullptr;

Vector2 Random2D::get_direction() {
	real_t t = randf() * Math_TAU;
	return Vector2(Math::cos(t), Math::sin(t));
}

real_t Random2D::get_rotation() {
	return randf_range(0.0, Math_TAU);
}

Vector2 Random2D::point_in_region(const Vector2 &p_pos, const Vector2 &p_end) {
	Point2 point;
	point.x = randf_range(p_pos.x, p_end.x);
	point.y = randf_range(p_pos.y, p_end.y);
	return point;
}

Vector2 Random2D::point_in_circle(real_t p_radius_min, real_t p_radius_max) {
	real_t r2_max = p_radius_max * p_radius_max;
	real_t r2_min = p_radius_min * p_radius_min;
	real_t r = Math::sqrt(randf() * (r2_max - r2_min) + r2_min);
	real_t t = randf() * Math_TAU;
	return Vector2(r * Math::cos(t), r * Math::sin(t));
}

Vector2 Random2D::point_in_triangle(const Vector<Point2> &p_tri) {
	ERR_FAIL_COND_V_MSG(p_tri.size() < 3, Vector2(), "Not a triangle");

	real_t s = randf();
	real_t t = randf();
	if (s + t > 1.0) {
		s = 1.0 - s;
		t = 1.0 - t;
	}
	return p_tri[0] * (1.0 - s - t) + p_tri[1] * s + p_tri[2] * t;
}

Variant Random2D::point_in_polygon(const Variant &p_polygon, int p_point_count) {
	ERR_FAIL_COND_V_MSG(p_point_count <= 0, Variant(), "Must generate at least one point.");

	Vector<Vector<Point2>> polygons_in;
	int point_count = p_point_count;

	switch (p_polygon.get_type()) {
		case Variant::POOL_VECTOR2_ARRAY: {
			Vector<Point2> poly = p_polygon;
			ERR_FAIL_COND_V_MSG(poly.size() < 3, Variant(), "Not a polygon.");
			polygons_in.push_back(poly);
		} break;
		case Variant::ARRAY: {
			// May contain both outer and inner polygons.
			Array polygons = p_polygon;
#ifdef DEBUG_ENABLED
			// Lets see if this actually contains polygons.
			for (int i = 0; i < polygons.size(); ++i) {
				if (polygons[i].get_type() != Variant::POOL_VECTOR2_ARRAY) {
					String name = Variant::get_type_name(polygons[i].get_type());
					ERR_PRINT(vformat("Invalid type: expected an Array of PoolVector2Array as polygons, not %s", name));
					break;
				}
			}
#endif
			for (int i = 0; i < polygons.size(); ++i) {
				const Vector<Point2> &poly = polygons[i];
				ERR_FAIL_COND_V_MSG(poly.size() < 3, Variant(), "Not a polygon.");
				polygons_in.push_back(poly);
			}
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Invalid type: expected either a single polygon or an array of polygons.");
		}
	}
	using TS = GoostGeometry2D;
	// `DECOMP_TRIANGLES_MONO` is mostly fail-proof for triangulation with holes,
	// but may have slightly worse distribution due to producing thin triangles.
	auto triangles = TS::decompose_polygons(TS::DECOMP_TRIANGLES_MONO, polygons_in);
	// Compute total positive and negative area for weighted distribution,
	// for both outer and inner polygons respectively.
	real_t total_area = 0.0;
	for (int i = 0; i < triangles.size(); ++i) {
		total_area += TS::polygon_area(triangles[i]);
	}
	if (p_point_count == 1) {
		// Pre-generate enough points with approximately uniform distribution,
		// and then simply pick a single random point from the array.
		// Shuffling triangles also works, but the quality of distribution
		// severely suffers in comparison.
		point_count = triangles.size() * 3;
	}
	// Get random points.
	Vector<Point2> points;
	for (int i = 0; i < triangles.size(); ++i) {
		const Vector<Point2> &tri = triangles[i];
		int n = Math::ceil(point_count * TS::polygon_area(tri) / total_area);
		for (int j = 0; j < n; ++j) {
			points.push_back(point_in_triangle(tri));
		}
	}
	Variant ret;
	if (points.size() == 1) {
		ret = points[0];
		return ret;
	}
	if (p_point_count == 1 && !points.empty()) {
		ret = points[randi() % points.size()];
		return ret;
	}
	if (points.size() > p_point_count) {
		points.resize(p_point_count);
	}
	ret = points;
	return ret;
}

void Random2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_direction"), &Random2D::get_direction);
	ClassDB::bind_method(D_METHOD("get_rotation"), &Random2D::get_rotation);

	ClassDB::bind_method(D_METHOD("point_in_region", "position_start", "position_end"), &Random2D::point_in_region);
	ClassDB::bind_method(D_METHOD("point_in_circle", "radius_min", "radius_max"), &Random2D::point_in_circle, DEFVAL(0.0), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("point_in_triangle", "triangle"), &Random2D::point_in_triangle);
	ClassDB::bind_method(D_METHOD("point_in_polygon", "polygon", "point_count"), &Random2D::point_in_polygon, DEFVAL(1));

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direction"), "", "get_direction");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation"), "", "get_rotation");

	// Default values are non-deterministic, override those for documentation purposes.	
	ADD_PROPERTY_DEFAULT("direction", Vector2(1, 0));
	ADD_PROPERTY_DEFAULT("rotation", Math_TAU);

	// Have to override in base class as well.
	ADD_PROPERTY_DEFAULT("number", 37);
	ADD_PROPERTY_DEFAULT("value", 0.5);
	ADD_PROPERTY_DEFAULT("color", Color(0, 0, 1));
	ADD_PROPERTY_DEFAULT("condition", true);
	ADD_PROPERTY_DEFAULT("seed", 0);
}
