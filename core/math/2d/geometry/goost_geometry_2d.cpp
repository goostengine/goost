#include "goost_geometry_2d.h"

#include "poly/boolean/clipper10/poly_boolean_clipper10.h"
#include "poly/boolean/clipper6/poly_boolean_clipper6.h"
#include "poly/decomp/clipper10/poly_decomp_clipper10.h"
#include "poly/decomp/polypartition/poly_decomp_polypartition.h"
#include "poly/offset/clipper10/poly_offset_clipper10.h"
#include "poly/offset/clipper6/poly_offset_clipper6.h"

PolyBoolean2D *GoostGeometry2D::poly_boolean = nullptr;
PolyOffset2D *GoostGeometry2D::poly_offset = nullptr;
PolyDecomp2D *GoostGeometry2D::poly_decomp = nullptr;

PolyBackend2DManager<PolyBoolean2D *> GoostGeometry2DManager::poly_boolean = PolyBackend2DManager<PolyBoolean2D *>();
PolyBackend2DManager<PolyOffset2D *> GoostGeometry2DManager::poly_offset = PolyBackend2DManager<PolyOffset2D *>();
PolyBackend2DManager<PolyDecomp2D *> GoostGeometry2DManager::poly_decomp = PolyBackend2DManager<PolyDecomp2D *>();

Ref<PolyBooleanParameters2D> GoostGeometry2D::default_poly_boolean_params = nullptr;
Ref<PolyOffsetParameters2D> GoostGeometry2D::default_poly_offset_params = nullptr;
Ref<PolyDecompParameters2D> GoostGeometry2D::default_poly_decomp_params = nullptr;

void GoostGeometry2D::initialize() {
	default_poly_boolean_params.instance();
	default_poly_offset_params.instance();
	default_poly_decomp_params.instance();

	GoostGeometry2DManager::poly_boolean.setting_name = "goost/geometry/2d/backends/poly_boolean";
	GoostGeometry2DManager::poly_offset.setting_name = "goost/geometry/2d/backends/poly_offset";
	GoostGeometry2DManager::poly_decomp.setting_name = "goost/geometry/2d/backends/poly_decomp";

	GoostGeometry2DManager::poly_boolean.register_backend("clipper6", memnew(PolyBoolean2DClipper6), true);
	GoostGeometry2DManager::poly_boolean.register_backend("clipper10", memnew(PolyBoolean2DClipper10));

	GoostGeometry2DManager::poly_offset.register_backend("clipper6", memnew(PolyOffset2DClipper6), true);
	GoostGeometry2DManager::poly_offset.register_backend("clipper10", memnew(PolyOffset2DClipper10));

	GoostGeometry2DManager::poly_decomp.register_backend("polypartition", memnew(PolyDecomp2DPolyPartition));
	GoostGeometry2DManager::poly_decomp.register_backend("clipper10:polypartition", memnew(PolyDecomp2DClipper10), true);

	GoostGeometry2DManager::poly_backends_changed_update();
}

void GoostGeometry2D::finalize() {
	default_poly_boolean_params.unref();
	default_poly_offset_params.unref();
	default_poly_decomp_params.unref();

	GoostGeometry2DManager::finalize();
}

Ref<PolyBooleanParameters2D> GoostGeometry2D::configure_boolean(const Ref<PolyBooleanParameters2D> &p_params) {
	default_poly_boolean_params->reset();
	if (p_params.is_valid()) {
		poly_boolean->set_params(p_params);
		return p_params;
	}
	poly_boolean->set_params(default_poly_boolean_params);
	return default_poly_boolean_params;
}

Ref<PolyOffsetParameters2D> GoostGeometry2D::configure_offset(const Ref<PolyOffsetParameters2D> &p_params) {
	default_poly_offset_params->reset();
	if (p_params.is_valid()) {
		poly_offset->set_params(p_params);
		return p_params;
	}
	poly_offset->set_params(default_poly_offset_params);
	return default_poly_offset_params;
}

Ref<PolyDecompParameters2D> GoostGeometry2D::configure_decomp(const Ref<PolyDecompParameters2D> &p_params) {
	default_poly_decomp_params->reset();
	if (p_params.is_valid()) {
		poly_decomp->set_params(p_params);
		return p_params;
	}
	poly_decomp->set_params(default_poly_decomp_params);
	return default_poly_decomp_params;
}

Vector<Vector<Point2>> GoostGeometry2D::merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	Vector<Vector<Point2>> polygons_a;
	polygons_a.push_back(p_polygon_a);
	Vector<Vector<Point2>> polygons_b;
	polygons_b.push_back(p_polygon_b);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_UNION, polygons_a, polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	Vector<Vector<Point2>> polygons_a;
	polygons_a.push_back(p_polygon_a);
	Vector<Vector<Point2>> polygons_b;
	polygons_b.push_back(p_polygon_b);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_DIFFERENCE, polygons_a, polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	Vector<Vector<Point2>> polygons_a;
	polygons_a.push_back(p_polygon_a);
	Vector<Vector<Point2>> polygons_b;
	polygons_b.push_back(p_polygon_b);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_INTERSECTION, polygons_a, polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	Vector<Vector<Point2>> polygons_a;
	polygons_a.push_back(p_polygon_a);
	Vector<Vector<Point2>> polygons_b;
	polygons_b.push_back(p_polygon_b);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_XOR, polygons_a, polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::merge_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_UNION, p_polygons_a, p_polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_DIFFERENCE, p_polygons_a, p_polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_INTERSECTION, p_polygons_a, p_polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::exclude_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_XOR, p_polygons_a, p_polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = true;
	Vector<Vector<Point2>> polylines;
	polylines.push_back(p_polyline);
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_DIFFERENCE, polylines, polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = true;
	Vector<Vector<Point2>> polylines;
	polylines.push_back(p_polyline);
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_INTERSECTION, polylines, polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_multiple_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = true;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_DIFFERENCE, p_polylines, p_polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_multiple_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = true;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::OPERATION_INTERSECTION, p_polylines, p_polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::polygons_boolean(PolyBooleanOperation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean(PolyBoolean2D::Operation(p_op), p_polygons_a, p_polygons_b);
}

Ref<PolyNode2D> GoostGeometry2D::polygons_boolean_tree(PolyBooleanOperation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params) {
	configure_boolean(p_params)->subject_open = false;
	return poly_boolean->polypaths_boolean_tree(PolyBoolean2D::Operation(p_op), p_polygons_a, p_polygons_b);
}

Vector<Vector<Point2>> GoostGeometry2D::inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_offset->offset_polypaths(polygons, -p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_offset->offset_polypaths(polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::inflate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	return poly_offset->offset_polypaths(p_polygons, -p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	return poly_offset->offset_polypaths(p_polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	Ref<PolyOffsetParameters2D> params = configure_offset(p_params);
	if (params->end_type == PolyOffsetParameters2D::END_POLYGON) {
		WARN_PRINT_ONCE("END_POLYGON does not apply for polyline deflating, fallback to END_JOINED.");
		params->end_type = PolyOffsetParameters2D::END_JOINED;
	}
	Vector<Vector<Point2>> polylines;
	polylines.push_back(p_polyline);
	return poly_offset->offset_polypaths(polylines, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_multiple_polylines(const Vector<Vector<Point2>> &p_polylines, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	Ref<PolyOffsetParameters2D> params = configure_offset(p_params);
	if (params->end_type == PolyOffsetParameters2D::END_POLYGON) {
		WARN_PRINT_ONCE("END_POLYGON does not apply for polyline deflating, fallback to END_JOINED.");
		params->end_type = PolyOffsetParameters2D::END_JOINED;
	}
	return poly_offset->offset_polypaths(p_polylines, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::offset_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_offset->offset_polypaths(polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::offset_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) {
	configure_offset(p_params)->end_type = PolyOffsetParameters2D::END_POLYGON;
	return poly_offset->offset_polypaths(p_polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::triangulate_polygon(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) {
	configure_decomp(p_params);
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_decomp->decompose_polygons(PolyDecomp2D::DECOMP_TRIANGLES_EC, polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::triangulate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params) {
	configure_decomp(p_params);
	return poly_decomp->decompose_polygons(PolyDecomp2D::DECOMP_TRIANGLES_EC, p_polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::decompose_polygon_into_convex(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) {
	configure_decomp(p_params);
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return poly_decomp->decompose_polygons(PolyDecomp2D::DECOMP_CONVEX_HM, polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::decompose_multiple_polygons_into_convex(const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params) {
	configure_decomp(p_params);
	return poly_decomp->decompose_polygons(PolyDecomp2D::DECOMP_CONVEX_HM, p_polygons);
}

Vector<Vector<Point2>> GoostGeometry2D::decompose_polygons(PolyDecompType p_type, const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params) {
	configure_decomp(p_params);
	return poly_decomp->decompose_polygons(PolyDecomp2D::DecompType(p_type), p_polygons);
}

Point2 GoostGeometry2D::polygon_centroid(const Vector<Point2> &p_polygon) {
	// Based on formulae from:
	// "Calculating The Area And Centroid Of A Polygon" Written by Paul Bourke July 1988
	// https://www.seas.upenn.edu/~sys502/extra_materials/Polygon%20Area%20and%20Centroid.pdf
	int c = p_polygon.size();
	ERR_FAIL_COND_V(c < 3, Vector2());

	const Vector2 *p = p_polygon.ptr();

	Vector2 centroid;
	real_t signed_area = 0.0;
	real_t a = 0.0;

	for (int i = 0; i < c; i++) {
		const Vector2 &v1 = p[i];
		const Vector2 &v2 = p[(i + 1) % c];
		a = v1.x * v2.y - v2.x * v1.y;
		signed_area += a;
		centroid += (v1 + v2) * a;
	}
	signed_area *= 0.5;
	centroid /= (6.0 * signed_area);

	return centroid;
}

real_t GoostGeometry2D::polygon_area(const Vector<Point2> &p_polygon) {
	if (p_polygon.size() < 3) {
		return 0.0;
	}
	real_t area = 0.0;
	for (int i = 0, j = p_polygon.size() - 1; i < p_polygon.size(); ++i) {
		area += (p_polygon[j].x + p_polygon[i].x) * (p_polygon[j].y - p_polygon[i].y);
		j = i;
	}
	return -area * 0.5;
}

real_t GoostGeometry2D::polygon_perimeter(const Vector<Point2> &p_polygon) {
	ERR_FAIL_COND_V(p_polygon.size() < 3, 0.0);

	real_t perimeter = 0.0;

	for (int i = 0; i < p_polygon.size(); ++i) {
		const Vector2 &v1 = p_polygon[i];
		const Vector2 &v2 = p_polygon[(i + 1) % p_polygon.size()];
		perimeter += Math::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}
	return perimeter;
}

real_t GoostGeometry2D::polyline_length(const Vector<Point2> &p_polyline) {
	ERR_FAIL_COND_V(p_polyline.size() < 2, 0.0);

	real_t length = 0.0;

	for (int i = 0; i < p_polyline.size() - 1; ++i) {
		const Vector2 &v1 = p_polyline[i];
		const Vector2 &v2 = p_polyline[i + 1];
		length += Math::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}
	return length;
}

// See "The Point in Polygon Problem for Arbitrary Polygons" by Hormann & Agathos
// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.88.5498&rep=rep1&type=pdf
//
// Implementation ported from Clipper 6.4.2.
//
int GoostGeometry2D::point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon) {
	ERR_FAIL_COND_V(p_polygon.size() < 3, 0);

	int pip_result = 0;
	Point2 ip = p_polygon[0];
	const Point2 &pt = p_point;

	for (int i = 1; i <= p_polygon.size(); ++i) {
		const Point2 &ip_next = i == p_polygon.size() ? p_polygon[0] : p_polygon[i];
		if (ip_next.y == pt.y) {
			if ((ip_next.x == pt.x) || (ip.y == pt.y && ((ip_next.x > pt.x) == (ip.x < pt.x)))) {
				return -1;
			}
		}
		if ((ip.y < pt.y) != (ip_next.y < pt.y)) {
			if (ip.x >= pt.x) {
				if (ip_next.x > pt.x) {
					pip_result = 1 - pip_result;
				} else {
					const real_t d = (ip.x - pt.x) * (ip_next.y - pt.y) - (ip_next.x - pt.x) * (ip.y - pt.y);
					if (!d) {
						return -1;
					}
					if ((d > 0) == (ip_next.y > ip.y)) {
						pip_result = 1 - pip_result;
					}
				}
			} else {
				if (ip_next.x > pt.x) {
					const real_t d = (ip.x - pt.x) * (ip_next.y - pt.y) - (ip_next.x - pt.x) * (ip.y - pt.y);
					if (!d) {
						return -1;
					}
					if ((d > 0) == (ip_next.y > ip.y)) {
						pip_result = 1 - pip_result;
					}
				}
			}
		}
		ip = ip_next;
	}
	return pip_result;
}

Vector<Point2> GoostGeometry2D::regular_polygon(int p_edge_count, real_t p_size) {
	ERR_FAIL_COND_V(p_edge_count < 3, Vector<Point2>());

	Vector<Point2> polygon;

	// Always generate polygons in counter-clockwise order by default.
	for (int i = p_edge_count - 1; i >= 0; --i) {
		const real_t phi = (Math_TAU / p_edge_count) * i;
		polygon.push_back(Vector2(Math::sin(phi), Math::cos(phi)) * p_size);
	}
	return polygon;
}

Vector<Point2> GoostGeometry2D::circle(real_t p_radius, real_t p_max_error) {
	ERR_FAIL_COND_V(p_radius < 0.0, Vector<Point2>());
	ERR_FAIL_COND_V(p_max_error < 0.0, Vector<Point2>());

	static const int max_points = 4096;

	int vertex_count = static_cast<int>(Math::ceil(Math_PI / Math::acos(1.0 - p_max_error / p_radius)));
	vertex_count = CLAMP(vertex_count, 3, max_points);

	return regular_polygon(vertex_count, p_radius); // vertex count == edge count
}

Vector2 GoostGeometry2D::rand_point_in_circle(real_t p_radius) {
	real_t r = Math::sqrt(Math::random(0.0, 1.0)) * p_radius;
	real_t t = Math::random(0.0, 1.0) * Math_TAU;
	return Vector2(r * Math::cos(t), r * Math::sin(t));
}

Vector2 GoostGeometry2D::rand_point_on_circle(real_t p_radius) {
	real_t t = Math::random(0.0, 1.0) * Math_TAU;
	return Vector2(p_radius * Math::cos(t), p_radius * Math::sin(t));
}

Vector2 GoostGeometry2D::rand_point_in_ring(real_t p_min_radius, real_t p_max_radius) {
	real_t r2_max = p_max_radius * p_max_radius;
	real_t r2_min = p_min_radius * p_min_radius;
	real_t r = Math::sqrt(Math::random(0.0, 1.0) * (r2_max - r2_min) + r2_min);
	real_t t = Math::random(0.0, 1.0) * Math_TAU;
	return Vector2(r * Math::cos(t), r * Math::sin(t));
}
