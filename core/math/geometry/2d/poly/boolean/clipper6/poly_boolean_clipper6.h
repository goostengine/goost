#pragma once

#include "../poly_boolean.h"
#include "thirdparty/misc/clipper.hpp"

class PolyBoolean2DClipper6 : public PolyBoolean2DBackend {
public:
	virtual Vector<Vector<Point2>> boolean_polypaths(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op);
	virtual void boolean_polypaths_tree(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op, PolyNode2D *r_root);

private:
	ClipperLib::Clipper configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_params);
	ClipperLib::ClipType clip_type;
	ClipperLib::PolyFillType subject_fill_type;
	ClipperLib::PolyFillType clip_fill_type;
	bool subject_open;
};

