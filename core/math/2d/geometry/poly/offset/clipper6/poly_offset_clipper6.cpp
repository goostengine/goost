#include "poly_offset_clipper6.h"
#include "goost/core/math/2d/geometry/poly/utils/godot_clipper6_path_convert.h"

Vector<Vector<Point2>> PolyOffset2DClipper6::offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta) {
	ClipperLib::ClipperOffset clp = configure(params);

	ClipperLib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths, subject);
	clp.AddPaths(subject, join_type, end_type);

	ClipperLib::Paths solution;
	clp.Execute(solution, p_delta * SCALE_FACTOR);

	Vector<Vector<Point2>> ret;
	GodotClipperUtils::scale_down_polypaths(solution, ret);

	return ret;
}

ClipperLib::ClipperOffset PolyOffset2DClipper6::configure(const Ref<PolyOffsetParameters2D> &p_params) {
	using namespace ClipperLib;

	switch (p_params->join_type) {
		case PolyOffsetParameters2D::JOIN_SQUARE:
			join_type = jtSquare;
			break;
		case PolyOffsetParameters2D::JOIN_ROUND:
			join_type = jtRound;
			break;
		case PolyOffsetParameters2D::JOIN_MITER:
			join_type = jtMiter;
			break;
	}
	switch (p_params->end_type) {
		case PolyOffsetParameters2D::END_POLYGON:
			end_type = etClosedPolygon;
			break;
		case PolyOffsetParameters2D::END_JOINED:
			end_type = etClosedLine;
			break;
		case PolyOffsetParameters2D::END_BUTT:
			end_type = etOpenButt;
			break;
		case PolyOffsetParameters2D::END_SQUARE:
			end_type = etOpenSquare;
			break;
		case PolyOffsetParameters2D::END_ROUND:
			end_type = etOpenRound;
			break;
	}
	return ClipperOffset(p_params->miter_limit, p_params->arc_tolerance * SCALE_FACTOR);
}
