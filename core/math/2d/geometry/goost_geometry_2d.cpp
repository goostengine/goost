#include "goost_geometry_2d.h"

#include "poly/boolean/poly_boolean.h"
#include "poly/decomp/poly_decomp.h"
#include "poly/offset/poly_offset.h"

#include "core/local_vector.h"

Vector<Vector<Point2>> GoostGeometry2D::merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polygon_a);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon_b);
	return PolyBoolean2D::merge_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polygon_a);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon_b);
	return PolyBoolean2D::clip_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polygon_a);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon_b);
	return PolyBoolean2D::intersect_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polygon_a);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon_b);
	return PolyBoolean2D::exclude_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polyline);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon);
	return PolyBoolean2D::clip_polylines_with_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon) {
	Vector<Vector<Point2>> subject;
	subject.push_back(p_polyline);
	Vector<Vector<Point2>> clip;
	clip.push_back(p_polygon);
	return PolyBoolean2D::intersect_polylines_with_polygons(subject, clip);
}

Vector<Vector<Point2>> GoostGeometry2D::inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return PolyOffset2D::inflate_polygons(polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return PolyOffset2D::deflate_polygons(polygons, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta) {
	ERR_FAIL_COND_V(p_delta < 0, Vector<Vector<Point2>>());
	Vector<Vector<Point2>> polylines;
	polylines.push_back(p_polyline);
	return PolyOffset2D::deflate_polylines(polylines, p_delta);
}

Vector<Vector<Point2>> GoostGeometry2D::triangulate_polygon(const Vector<Point2> &p_polygon) {
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	// Using DECOMP_TRIANGLES_MONO as it's mostly fail-proof in clipper10 backend (default).
	return PolyDecomp2D::decompose_polygons(polygons, PolyDecomp2D::DECOMP_TRIANGLES_MONO);
}

Vector<Vector<Point2>> GoostGeometry2D::decompose_polygon(const Vector<Point2> &p_polygon) {
	Vector<Vector<Point2>> polygons;
	polygons.push_back(p_polygon);
	return PolyDecomp2D::decompose_polygons(polygons, PolyDecomp2D::DECOMP_CONVEX_HM);
}

struct IndicesStack {
	LocalVector<int> stack;
	uint32_t stack_size = 0;
	uint32_t back = 0;

	_FORCE_INLINE_ void push_back(int p_index) {
		if (stack.size() == back) {
			stack.push_back(p_index);
		} else {
			stack[back] = p_index;
		}
		++back;
		++stack_size;
	}
	_FORCE_INLINE_ int pop_back() {
		--stack_size;
		return stack[--back];
	}
	_FORCE_INLINE_ bool is_empty() {
		return stack_size == 0;
	}
	_FORCE_INLINE_ const int &operator[](int p_index) const {
		return stack[p_index];
	}
};

// Polyline decimation using Ramer-Douglas-Peucker (RDP) algorithm.
Vector<Point2> GoostGeometry2D::simplify_polyline(const Vector<Point2> &p_polyline, real_t p_epsilon) {
	if (p_polyline.size() <= 2) {
		return p_polyline;
	}
	Vector<bool> points_to_retain;
	points_to_retain.resize(p_polyline.size());

	bool *retain = points_to_retain.ptrw();
	memset(retain, 0, points_to_retain.size());

	real_t eps = MAX(0.0, p_epsilon);
	real_t distance_max = 0;
	real_t distance = 0;

	IndicesStack parts;
	parts.stack.reserve(p_polyline.size() * 2);
	parts.push_back(0);
	parts.push_back(p_polyline.size() - 1);

	retain[parts[0]] = true;
	retain[parts[1]] = true;
	int index = 0;

	while (!parts.is_empty()) {
		int second = parts.pop_back(); // Pop back in other order.
		int first = parts.pop_back();

		distance_max = 0;
		Vector2 a = p_polyline[first];
		Vector2 b = p_polyline[second];
		Vector2 n = b - a;

		for (int i = first + 1; i < second; ++i) {
			Vector2 pa = a - p_polyline[i];
			Vector2 c = n * pa.dot(n) / n.dot(n);
			Vector2 d = pa - c;
			distance = d.dot(d);
			if (distance > distance_max) {
				index = i;
				distance_max = distance;
			}
		}
		if (distance_max >= eps) {
			retain[index] = true;
			parts.push_back(first);
			parts.push_back(index);
			parts.push_back(index);
			parts.push_back(second);
		}
	}
	Vector<Vector2> ret;
	for (int i = 0; i < p_polyline.size(); ++i) {
		if (retain[i]) {
			ret.push_back(p_polyline[i]);
		}
	}
	return ret;
}

// Approximate polygon smoothing using Chaikin algorithm.
// https://www.cs.unc.edu/~dm/UNC/COMP258/LECTURES/Chaikins-Algorithm.pdf
//
Vector<Point2> GoostGeometry2D::smooth_polygon_approx(const Vector<Point2> &p_polygon, int p_iterations, real_t p_cut_distance) {
	ERR_FAIL_COND_V_MSG(p_polygon.size() < 3, Vector<Point2>(), "Bad polygon!");

	Vector<Point2> subject = p_polygon;
	const real_t cd = CLAMP(p_cut_distance, 0.0, 0.5);
	for (int i = 0; i < p_iterations; ++i) {
		Vector<Point2> smoothed;
		for (int j = 0; j < subject.size(); ++j) {
			const Point2 &p1 = subject[j];
			const Point2 &p2 = subject[(j + 1) % subject.size()];
			smoothed.push_back((1.0 - cd) * p1 + cd * p2); // Q
			smoothed.push_back(cd * p1 + (1.0 - cd) * p2); // R
		}
		subject = smoothed;
	}
	return subject;
}

// Approximate polyline smoothing using Chaikin algorithm:
// https://www.cs.unc.edu/~dm/UNC/COMP258/LECTURES/Chaikins-Algorithm.pdf
//
// Unlike polygon version, the endpoints are always retained.
//
Vector<Point2> GoostGeometry2D::smooth_polyline_approx(const Vector<Point2> &p_polyline, int p_iterations, real_t p_cut_distance) {
	if (p_polyline.size() <= 2) {
		return p_polyline;
	}
	Vector<Point2> subject = p_polyline;
	const real_t cd = CLAMP(p_cut_distance, 0.0, 0.5);
	for (int i = 0; i < p_iterations; ++i) {
		Vector<Point2> smoothed;
		smoothed.push_back(subject[0]); // Always add first point.
		for (int j = 0; j < subject.size() - 1; ++j) {
			const Point2 &p1 = subject[j];
			const Point2 &p2 = subject[j + 1];
			smoothed.push_back((1.0 - cd) * p1 + cd * p2); // Q
			smoothed.push_back(cd * p1 + (1.0 - cd) * p2); // R
		}
		smoothed.push_back(subject[subject.size() - 1]); // Always add last point.
		subject = smoothed;
	}
	return subject;
}

// "Calculating The Area And Centroid Of A Polygon" Written by Paul Bourke July 1988
// https://www.seas.upenn.edu/~sys502/extra_materials/Polygon%20Area%20and%20Centroid.pdf
//
Point2 GoostGeometry2D::polygon_centroid(const Vector<Point2> &p_polygon) {
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

Rect2 GoostGeometry2D::bounding_rect(const Vector<Point2> &p_points) {
	Rect2 rect;
	for (int i = 0; i < p_points.size(); ++i) {
		if (i == 0) {
			rect = Rect2(p_points[i], Size2());
		} else {
			rect.expand_to(p_points[i]);
		}
	}
	return rect;
}

// "The Point in Polygon Problem for Arbitrary Polygons" by Hormann & Agathos
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

// Implementation borrowed from `TileMap` editor plugin:
// https://github.com/godotengine/godot/blob/0d819ae5f5/editor/plugins/tile_map_editor_plugin.cpp#L982-L1023
//
Vector<Point2i> GoostGeometry2D::bresenham_line(const Point2i &p_start, const Point2i &p_end) {
	Vector<Point2i> points;

	real_t dx = ABS(p_end.x - p_start.x);
	real_t dy = ABS(p_end.y - p_start.y);

	int x = p_start.x;
	int y = p_start.y;

	int sx = p_start.x > p_end.x ? -1 : 1;
	int sy = p_start.y > p_end.y ? -1 : 1;

	if (dx > dy) {
		real_t err = dx / 2;
		for (; x != p_end.x; x += sx) {
			points.push_back(Point2i(x, y));
			err -= dy;
			if (err < 0) {
				y += sy;
				err += dx;
			}
		}
	} else {
		real_t err = dy / 2;
		for (; y != p_end.y; y += sy) {
			points.push_back(Point2i(x, y));
			err -= dx;
			if (err < 0) {
				x += sx;
				err += dy;
			}
		}
	}
	points.push_back(Point2i(x, y));
	return points;
}
