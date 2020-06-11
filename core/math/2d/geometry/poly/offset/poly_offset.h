#ifndef GOOST_GEOMETRY_POLY_OFFSET_H
#define GOOST_GEOMETRY_POLY_OFFSET_H

#include "core/reference.h"

class PolyOffsetParameters2D;

class PolyOffset2D {
public:
	// Combines both polygon and polyline offseting.
	virtual Vector<Vector<Point2>> offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta) = 0;
	// virtual Ref<PolyNode2D> offset_polypaths_tree(const Vector<Vector<Point2> > &p_polypaths, real_t p_delta) = 0;

	virtual ~PolyOffset2D() {}

public:
	void set_params(const Ref<PolyOffsetParameters2D> &p_params) { params = p_params; }

protected:
	Ref<PolyOffsetParameters2D> params;
};

class PolyOffsetParameters2D : public Reference {
	GDCLASS(PolyOffsetParameters2D, Reference);

public:
	enum JoinType {
		JOIN_SQUARE,
		JOIN_ROUND,
		JOIN_MITER,
	};
	enum EndType {
		END_POLYGON,
		END_JOINED,
		END_BUTT,
		END_SQUARE,
		END_ROUND,
	};

public:
	JoinType join_type = JOIN_SQUARE;
	EndType end_type = END_SQUARE;
	real_t arc_tolerance = 0.25;
	real_t miter_limit = 2.0;

protected:
	static void _bind_methods();

public:
	void set_join_type(JoinType p_join_type) { join_type = p_join_type; }
	JoinType get_join_type() const { return join_type; }

	void set_end_type(EndType p_end_type) { end_type = p_end_type; }
	EndType get_end_type() const { return end_type; }

	void set_arc_tolerance(real_t p_arc_tolerance) { arc_tolerance = p_arc_tolerance; }
	real_t get_arc_tolerance() const { return arc_tolerance; }

	void set_miter_limit(real_t p_miter_limit) { miter_limit = p_miter_limit; }
	real_t get_miter_limit() const { return miter_limit; }

	void reset() {
		join_type = JOIN_SQUARE;
		end_type = END_SQUARE;
		arc_tolerance = 0.25;
		miter_limit = 2.0;
	}
};

VARIANT_ENUM_CAST(PolyOffsetParameters2D::JoinType);
VARIANT_ENUM_CAST(PolyOffsetParameters2D::EndType);

#endif // GOOST_GEOMETRY_POLY_OFFSET_H
