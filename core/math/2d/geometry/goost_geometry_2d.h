#ifndef GOOST_GEOMETRY_2D_H
#define GOOST_GEOMETRY_2D_H

#include "core/object.h"
#include "core/project_settings.h"

#include "poly/boolean/poly_boolean.h"
#include "poly/decomp/poly_decomp.h"
#include "poly/offset/poly_offset.h"

class GoostGeometry2D {
public:
	enum PolyBooleanOperation {
		OPERATION_NONE, // May perform polygons fixup, build hierarchy etc.
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	/* Polygon and polyline boolean operations */
	static Vector<Vector<Point2>> merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params = nullptr);

	static Vector<Vector<Point2>> merge_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> clip_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> intersect_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> exclude_multiple_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Ref<PolyBooleanParameters2D> p_params = nullptr);

	// General-purpose polygon boolean operations.
	static Vector<Vector<Point2>> polygons_boolean(PolyBooleanOperation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), Ref<PolyBooleanParameters2D> p_params = nullptr);

	// Returns a top-level root node which represents an hierarchy of polygons.
	static Ref<PolyNode2D> polygons_boolean_tree(PolyBooleanOperation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), Ref<PolyBooleanParameters2D> p_params = nullptr);

	// Note: UNION and XOR do not apply here.
	static Vector<Vector<Point2>> clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> clip_multiple_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, Ref<PolyBooleanParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> intersect_multiple_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, Ref<PolyBooleanParameters2D> p_params = nullptr);

	/* Polygon inflating and deflating */
	static Vector<Vector<Point2>> inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> inflate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> deflate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);

	// Grows polyline into a polygon.
	// Note: negative delta does not apply here.
	static Vector<Vector<Point2>> deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> deflate_multiple_polylines(const Vector<Vector<Point2>> &p_polylines, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);

	// General-purpose offsetting, accepts both negative and positive delta.
	static Vector<Vector<Point2>> offset_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> offset_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params = nullptr);

	/* Polygon decomposition */
	enum PolyDecompType {
		DECOMP_TRIANGLES_EC, // Ear clipping algorithm.
		DECOMP_TRIANGLES_OPT, // Minimal edge length.
		DECOMP_TRIANGLES_MONO, // Monotone polygon partitioning, then triangulate.
		DECOMP_CONVEX_HM, // Hertel-Mehlhorn algorithm.
		DECOMP_CONVEX_OPT, // Minimal number of convex polygons.
	};
	static Vector<Vector<Point2>> triangulate_polygon(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> triangulate_multiple_polygons(const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> decompose_polygon_into_convex(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params = nullptr);
	static Vector<Vector<Point2>> decompose_multiple_polygons_into_convex(const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params = nullptr);

	// General-purpose polygon decomposition.
	static Vector<Vector<Point2>> decompose_polygons(PolyDecompType p_type, const Vector<Vector<Point2>> &p_polygons, Ref<PolyDecompParameters2D> p_params = nullptr);

	/* Polygon/Polyline attributes */
	static Point2 polygon_centroid(const Vector<Point2> &p_polygon);
	static real_t polygon_area(const Vector<Point2> &p_polygon);
	static real_t polygon_perimeter(const Vector<Point2> &p_polygon);
	static real_t polyline_length(const Vector<Point2> &p_polyline);

	// Returns 0 if false, +1 if true, -1 if point is exactly on the polygon's boundary.
	static int point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon);

	/* Polygon/shapes generation methods */
	static Vector<Point2> regular_polygon(int p_edge_count, real_t p_size);
	static Vector<Point2> circle(real_t p_radius, real_t p_max_error = 0.25);

public:
	static void initialize();
	static void finalize();

	static void set_poly_boolean_instance(PolyBoolean2D *p_instance) { poly_boolean = p_instance; }
	static void set_poly_offset_instance(PolyOffset2D *p_instance) { poly_offset = p_instance; }
	static void set_poly_decomp_instance(PolyDecomp2D *p_instance) { poly_decomp = p_instance; }

protected:
	static Ref<PolyBooleanParameters2D> configure_boolean(const Ref<PolyBooleanParameters2D> &p_params);
	static Ref<PolyOffsetParameters2D> configure_offset(const Ref<PolyOffsetParameters2D> &p_params);
	static Ref<PolyDecompParameters2D> configure_decomp(const Ref<PolyDecompParameters2D> &p_params);

private:
	static PolyBoolean2D *poly_boolean;
	static PolyOffset2D *poly_offset;
	static PolyDecomp2D *poly_decomp;

	static Ref<PolyBooleanParameters2D> default_poly_boolean_params;
	static Ref<PolyOffsetParameters2D> default_poly_offset_params;
	static Ref<PolyDecompParameters2D> default_poly_decomp_params;
};

template <class T>
class PolyBackend2DManager {
	struct Backend {
		String name = "";
		T instance = nullptr;

		Backend() :
				name(""),
				instance(nullptr) {}

		Backend(String p_name, T p_instance) :
				name(p_name),
				instance(p_instance) {}
	};

	Vector<Backend> backends;
	int default_backend_id = -1;
	String default_backend_name;

public:
	String setting_name;

public:
	void register_backend(const String &p_name, T p_instance, bool p_default = false) {
		backends.push_back(Backend(p_name, p_instance));
		if (p_default) {
			set_default_backend(p_name);
		}
	}
	void set_default_backend(const String &p_name) {
		default_backend_name = p_name;
		default_backend_id = find_backend_id(p_name);
	}

	T get_default_backend_instance() const {
		if (default_backend_id != -1) {
			return backends[default_backend_id].instance;
		}
		return nullptr;
	}

	int find_backend_id(const String &p_name) const {
		for (int i = 0; i < backends.size(); ++i) {
			if (p_name == backends[i].name) {
				return i;
			}
		}
		return -1;
	}

	T get_backend_instance(const String &p_name) const {
		int id = find_backend_id(p_name);
		if (id != -1) {
			return backends[id].instance;
		}
		return nullptr;
	}

	int get_backends_count() const { return backends.size(); }

	String get_backend_name(int p_id) const {
		CRASH_BAD_INDEX(p_id, backends.size());
		return backends[p_id].name;
	}

	String update() {
		String backends_list;

		for (int i = 0; i < get_backends_count(); ++i) {
			backends_list += get_backend_name(i);
			if (i < get_backends_count() - 1) {
				backends_list += ",";
			}
		}
		T default_backend = get_default_backend_instance();
		if (default_backend) {
			// Suggest restart because the singleton can also be used in extension mode.
			GLOBAL_DEF_RST(setting_name, default_backend_name);
		}
		if (!backends_list.empty()) {
			ProjectSettings::get_singleton()->set_custom_property_info(
					setting_name,
					PropertyInfo(Variant::STRING, setting_name, PROPERTY_HINT_ENUM, backends_list));
		}
		return ProjectSettings::get_singleton()->has_setting(setting_name) ? GLOBAL_GET(setting_name) : "";
	}

	void finalize() {
		for (int i = 0; i < backends.size(); ++i) {
			if (backends[i].instance) {
				memdelete(backends[i].instance);
			}
		}
	}
};

class GoostGeometry2DManager {
public:
	static PolyBackend2DManager<PolyBoolean2D *> poly_boolean;
	static PolyBackend2DManager<PolyOffset2D *> poly_offset;
	static PolyBackend2DManager<PolyDecomp2D *> poly_decomp;

	static void poly_backends_changed_update() {
		String selected;
		selected = poly_boolean.update();
		GoostGeometry2D::set_poly_boolean_instance(poly_boolean.get_backend_instance(selected));
		selected = poly_offset.update();
		GoostGeometry2D::set_poly_offset_instance(poly_offset.get_backend_instance(selected));
		selected = poly_decomp.update();
		GoostGeometry2D::set_poly_decomp_instance(poly_decomp.get_backend_instance(selected));
	}
	static void finalize() {
		poly_boolean.finalize();
		poly_offset.finalize();
		poly_decomp.finalize();
	}
};

#endif // GOOST_GEOMETRY_2D_H
