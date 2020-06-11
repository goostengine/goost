#ifndef GOOST_GEOMETRY_POLY_DECOMP_POLYPARTITION_H
#define GOOST_GEOMETRY_POLY_DECOMP_POLYPARTITION_H

#include "../poly_decomp.h"
#include "core/reference.h"

class PolyDecompParameters2D;

class PolyDecomp2DPolyPartition : public PolyDecomp2D {
public:
	virtual Vector<Vector<Point2>> triangulate_ec(const Vector<Vector<Point2>> &p_polygons) override;
	virtual Vector<Vector<Point2>> triangulate_opt(const Vector<Vector<Point2>> &p_polygons) override;
	virtual Vector<Vector<Point2>> triangulate_mono(const Vector<Vector<Point2>> &p_polygons) override;
	virtual Vector<Vector<Point2>> decompose_convex_hm(const Vector<Vector<Point2>> &p_polygons) override;
	virtual Vector<Vector<Point2>> decompose_convex_opt(const Vector<Vector<Point2>> &p_polygons) override;
};

#endif // GOOST_GEOMETRY_POLY_DECOMP_POLYPARTITION_H
