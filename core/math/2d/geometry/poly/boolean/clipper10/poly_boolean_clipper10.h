#ifndef GOOST_GEOMETRY_CLIPPER10
#define GOOST_GEOMETRY_CLIPPER10

#include "goost/core/math/2d/geometry/poly/boolean/poly_boolean.h"
#include "goost/thirdparty/clipper/clipper.h"

class PolyBoolean2DClipper10 : public PolyBoolean2D {
public:
	virtual Vector<Vector<Point2>> polypaths_boolean(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) override;
	virtual Ref<PolyNode2D> polypaths_boolean_tree(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) override;

protected:
	clipperlib::Clipper configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_params);

private:
	clipperlib::ClipType clip_type;
	clipperlib::FillRule subject_fill_rule;
	clipperlib::FillRule clip_fill_rule;
	bool subject_open;
};

#endif // GOOST_GEOMETRY_CLIPPER10
