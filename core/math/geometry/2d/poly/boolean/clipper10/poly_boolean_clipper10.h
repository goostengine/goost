#pragma once

#include "../poly_boolean.h"
#include "goost/thirdparty/clipper/clipper.h"

class PolyBoolean2DClipper10 : public PolyBoolean2DBackend {
public:
	virtual Vector<Vector<Point2>> boolean_polypaths(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op);
	virtual void boolean_polypaths_tree(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op, PolyNode2D *r_root);

private:
	clipperlib::Clipper configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_params);
	clipperlib::ClipType clip_type;
	clipperlib::FillRule subject_fill_rule;
	clipperlib::FillRule clip_fill_rule;
	bool subject_open;
};

