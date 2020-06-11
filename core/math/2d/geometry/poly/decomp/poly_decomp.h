#ifndef GOOST_GEOMETRY_POLY_DECOMP_H
#define GOOST_GEOMETRY_POLY_DECOMP_H

#include "core/reference.h"

class PolyDecompParameters2D;

class PolyDecomp2D {
public:
	enum DecompType {
		DECOMP_TRIANGLES_EC,
		DECOMP_TRIANGLES_OPT,
		DECOMP_TRIANGLES_MONO,
		DECOMP_CONVEX_HM,
		DECOMP_CONVEX_OPT,
	};
	virtual Vector<Vector<Point2>> decompose_polygons(DecompType p_type, const Vector<Vector<Point2>> &p_polygons);

	virtual Vector<Vector<Point2>> triangulate_ec(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> triangulate_opt(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> triangulate_mono(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> decompose_convex_hm(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> decompose_convex_opt(const Vector<Vector<Point2>> &p_polygons) = 0;

	virtual ~PolyDecomp2D() {}

public:
	void set_params(const Ref<PolyDecompParameters2D> &p_params) { params = p_params; }

protected:
	Ref<PolyDecompParameters2D> params;
};

class PolyDecompParameters2D : public Reference {
	GDCLASS(PolyDecompParameters2D, Reference);

public:
	enum FillRule {
		FILL_RULE_EVEN_ODD,
		FILL_RULE_NON_ZERO,
		FILL_RULE_POSITIVE,
		FILL_RULE_NEGATIVE,
	};

public:
	FillRule fill_rule = FILL_RULE_NON_ZERO;
	// Steiner points...
	// Inner polygons indices...

protected:
	static void _bind_methods();

public:
	void set_fill_rule(FillRule p_fill_rule) { fill_rule = p_fill_rule; }
	FillRule get_fill_rule() const { return fill_rule; }

	void reset() {
		fill_rule = FILL_RULE_NON_ZERO;
	}
};

VARIANT_ENUM_CAST(PolyDecompParameters2D::FillRule);

#endif // GOOST_GEOMETRY_POLY_DECOMP_H
