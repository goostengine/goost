#ifndef GOOST_GEOMETRY_TOOLS_BIND_H
#define GOOST_GEOMETRY_TOOLS_BIND_H

#include "goost/core/math/2d/geometry/geometry_extension.h"

class _GeometryExtension2D : public Object {
	GDCLASS(_GeometryExtension2D, Object);

private:
	static _GeometryExtension2D *singleton;

protected:
	static void _bind_methods();

public:
	static _GeometryExtension2D *get_singleton() { return singleton; }
	
public:
	enum PolyBooleanOperation {
		OPERATION_NONE,
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	Array merge_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array clip_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array intersect_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array exclude_polygons(const Vector<Point2> &p_polygon_a, const Vector<Point2> &p_polygon_b, Ref<PolyBooleanParameters2D> p_params) const;
	
	Array merge_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array clip_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array intersect_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;
	Array exclude_multiple_polygons(Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;

	Array polygons_boolean(PolyBooleanOperation p_op, Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;
	Ref<PolyNode2D> polygons_boolean_tree(PolyBooleanOperation p_op, Array p_polygons_a, Array p_polygons_b, Ref<PolyBooleanParameters2D> p_params) const;
	
	Array clip_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) const;
	Array intersect_polyline_with_polygon(const Vector<Point2> &p_polyline, const Vector<Point2> &p_polygon, Ref<PolyBooleanParameters2D> p_params) const;
	Array clip_multiple_polylines_with_polygons(const Array &p_polylines, const Array &p_polygons, Ref<PolyBooleanParameters2D> p_params) const;
	Array intersect_multiple_polylines_with_polygons(const Array &p_polylines, const Array &p_polygons, Ref<PolyBooleanParameters2D> p_params) const;
	
	Array inflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	Array deflate_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	Array inflate_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	Array deflate_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	
	Array deflate_polyline(const Vector<Point2> &p_polyline, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	Array deflate_multiple_polylines(const Array &p_polylines, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	
	Array offset_polygon(const Vector<Point2> &p_polygon, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	Array offset_multiple_polygons(const Array &p_polygons, real_t p_delta, Ref<PolyOffsetParameters2D> p_params) const;
	
	enum PolyDecompType {
		DECOMP_TRIANGLES_EC, 
		DECOMP_TRIANGLES_OPT, 
		DECOMP_TRIANGLES_MONO,
		DECOMP_CONVEX_HM,
		DECOMP_CONVEX_OPT,
	};
	Array triangulate_polygon(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) const;
	Array triangulate_multiple_polygons(const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const;
	Array decompose_polygon_into_convex(const Vector<Point2> &p_polygon, Ref<PolyDecompParameters2D> p_params) const;
	Array decompose_multiple_polygons_into_convex(const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const;

	Array decompose_polygons(PolyDecompType p_type, const Array &p_polygons, Ref<PolyDecompParameters2D> p_params) const;

	Vector2 polygon_centroid(const Vector<Vector2> &p_polygon) const;
	real_t polygon_area(const Vector<Vector2> &p_polygon) const;
	real_t polygon_perimeter(const Vector<Vector2> &p_polygon) const;
	real_t polyline_length(const Vector<Vector2> &p_polyline) const;
	
	int point_in_polygon(const Point2 &p_point, const Vector<Point2> &p_polygon) const;
	
	Vector<Point2> regular_polygon(int p_edge_count, real_t p_size) const;
	Vector<Point2> circle(real_t p_radius, real_t p_max_error) const;
	
	_GeometryExtension2D();
};

VARIANT_ENUM_CAST(_GeometryExtension2D::PolyBooleanOperation);
VARIANT_ENUM_CAST(_GeometryExtension2D::PolyDecompType);

#endif // GOOST_GEOMETRY_TOOLS_BIND_H
