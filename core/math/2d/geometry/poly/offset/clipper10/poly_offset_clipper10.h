#ifndef GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10
#define GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10

#include "goost/core/math/2d/geometry/poly/offset/poly_offset.h"
#include "goost/thirdparty/clipper/clipper_offset.h"

class PolyOffset2DClipper10 : public PolyOffset2D {
public:
	virtual Vector<Vector<Point2> > offset_polypaths(const Vector<Vector<Point2> > &p_polypaths, real_t p_delta) override;
	
protected:
	clipperlib::ClipperOffset configure(const Ref<PolyOffsetParameters2D> &p_params);
	
private:
	clipperlib::JoinType join_type;
	clipperlib::EndType end_type;
};

#endif // GOOST_GEOMETRY_POLY_OFFSET_CLIPPER10
