#ifndef GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10
#define GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10

#include "../poly_offset.h"
#include "goost/thirdparty/clipper/clipper_offset.h"

class PolyOffset2DClipper10 : public PolyOffset2DBackend {
public:
	virtual Vector<Vector<Point2>> offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta) override;

private:
	clipperlib::ClipperOffset configure(const Ref<PolyOffsetParameters2D> &p_parameters);
	clipperlib::JoinType join_type;
	clipperlib::EndType end_type;
};

#endif // GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10
