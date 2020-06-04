#ifndef GOOST_GEOMETRY_POLY_DECOMP_CLIPPER10
#define GOOST_GEOMETRY_POLY_DECOMP_CLIPPER10

#include "goost/core/math/2d/geometry/poly/decomp/polypartition/poly_decomp_polypartition.h"
#include "goost/thirdparty/clipper/clipper_triangulation.h"

class PolyDecomp2DClipper10 : public PolyDecomp2DPolyPartition {
public:
	// Seems like Clipper 10.0.0 uses monotone partitioning (?)
	virtual Vector<Vector<Point2> > triangulate_mono(const Vector<Vector<Point2> > &p_polygons) override;
	
protected:
	clipperlib::ClipperTri configure(const Ref<PolyDecompParameters2D> &p_params);

private:
	clipperlib::FillRule fill_rule;
};

#endif // GOOST_GEOMETRY_POLY_DECOMP_CLIPPER10
