#ifndef GOOST_GEOMETRY_POLY_BOOLEAN_CLIPPER6_H
#define GOOST_GEOMETRY_POLY_BOOLEAN_CLIPPER6_H

#include "goost/core/math/2d/geometry/poly/boolean/poly_boolean.h"
#include "thirdparty/misc/clipper.hpp"

class PolyBoolean2DClipper6 : public PolyBoolean2D {
public:
	virtual Vector<Vector<Point2>> polypaths_boolean(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) override;
	virtual Ref<PolyNode2D> polypaths_boolean_tree(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) override;

protected:
	ClipperLib::Clipper configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_params);

private:
	ClipperLib::ClipType clip_type;
	ClipperLib::PolyFillType subject_fill_type;
	ClipperLib::PolyFillType clip_fill_type;
	bool subject_open;
};

#endif // GOOST_GEOMETRY_POLY_BOOLEAN_CLIPPER6_H
