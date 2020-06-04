#include "goost/core/math/2d/geometry/poly/utils/godot_clipper10_path_convert.h"
#include "poly_offset_clipper10.h"

Vector<Vector<Point2> > PolyOffset2DClipper10::offset_polypaths(const Vector<Vector<Point2> > &p_polypaths, real_t p_delta) {
	clipperlib::ClipperOffset clp = configure(params);
	
	clipperlib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths, subject);
	clp.AddPaths(subject, join_type, end_type);
	
	clipperlib::Paths solution;
	clp.Execute(solution, p_delta * SCALE_FACTOR);
	
	Vector<Vector<Point2> > ret;
	GodotClipperUtils::scale_down_polypaths(solution, ret);

	return ret;
}

clipperlib::ClipperOffset PolyOffset2DClipper10::configure(const Ref<PolyOffsetParameters2D> &p_params) {
	using namespace clipperlib;

	switch (p_params->join_type) {
		case PolyOffsetParameters2D::JOIN_SQUARE: join_type = kSquare; break;
		case PolyOffsetParameters2D::JOIN_ROUND: join_type = kRound; break;
		case PolyOffsetParameters2D::JOIN_MITER: join_type = kMiter; break;
	}
	switch (p_params->end_type) {
		case PolyOffsetParameters2D::END_POLYGON: end_type = kPolygon; break;
		case PolyOffsetParameters2D::END_JOINED: end_type = kOpenJoined; break;
		case PolyOffsetParameters2D::END_BUTT: end_type = kOpenButt; break;
		case PolyOffsetParameters2D::END_SQUARE: end_type = kOpenSquare; break;
		case PolyOffsetParameters2D::END_ROUND: end_type = kOpenRound; break;
	}
	return ClipperOffset(p_params->miter_limit, p_params->arc_tolerance * SCALE_FACTOR);
}
