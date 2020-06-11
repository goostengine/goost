#ifndef GOOST_GEOMETRY_POLY_OFFSET_CLIPPER6
#define GOOST_GEOMETRY_POLY_OFFSET_CLIPPER6

#include "goost/core/math/2d/geometry/poly/offset/poly_offset.h"
#include "thirdparty/misc/clipper.hpp"

class PolyOffset2DClipper6 : public PolyOffset2D {
public:
	virtual Vector<Vector<Point2>> offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta) override;

protected:
	ClipperLib::ClipperOffset configure(const Ref<PolyOffsetParameters2D> &p_params);

private:
	ClipperLib::JoinType join_type;
	ClipperLib::EndType end_type;
};

#endif // GOOST_GEOMETRY_POLY_OFFSET_CLIPPER6
