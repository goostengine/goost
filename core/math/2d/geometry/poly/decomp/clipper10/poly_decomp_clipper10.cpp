#include "poly_decomp_clipper10.h"
#include "goost/core/math/2d/geometry/poly/utils/godot_clipper10_path_convert.h"

Vector<Vector<Point2>> PolyDecomp2DClipper10::triangulate_mono(const Vector<Vector<Point2>> &p_polygons) {
	using namespace clipperlib;

	ClipperTri clp = configure(parameters);

	Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polygons, subject);
	clp.AddPaths(subject, ptSubject);

	Paths triangles;
	clp.Execute(ctUnion, triangles, fill_rule);

	Vector<Vector<Point2>> ret;
	GodotClipperUtils::scale_down_polypaths(triangles, ret);

	return ret;
}

clipperlib::ClipperTri PolyDecomp2DClipper10::configure(const Ref<PolyDecompParameters2D> &p_parameters) {
	using namespace clipperlib;

	fill_rule = FillRule(p_parameters->fill_rule);

	return ClipperTri();
}
