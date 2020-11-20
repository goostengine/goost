#ifndef GOOST_POLY_SHAPES_2D_H
#define GOOST_POLY_SHAPES_2D_H

#include "goost/core/math/2d/geometry/poly/poly_node_2d.h"

class PolyCircle2D : public PolyNode2D {
	GDCLASS(PolyCircle2D, PolyNode2D);

	real_t radius = 32.0;
	real_t max_error = 0.25;

protected:
	static void _bind_methods();
	virtual Vector<Vector<Point2>> _build_outlines();

public:
	void set_radius(real_t p_radius);
	real_t get_radius() const { return radius; }

	void set_max_error(real_t p_max_error);
	real_t get_max_error() const { return max_error; }

	PolyCircle2D::PolyCircle2D() {
		build_outlines();
	}
};

class PolyRectangle2D : public PolyNode2D {
	GDCLASS(PolyRectangle2D, PolyNode2D);

	Vector2 extents = Vector2(32, 32);

protected:
	static void _bind_methods();
	virtual Vector<Vector<Point2>> _build_outlines();

public:
	void set_extents(const Vector2 &p_extents);
	Vector2 get_extents() const { return extents; }

	PolyRectangle2D::PolyRectangle2D() {
		build_outlines();
	}
};

#endif // GOOST_POLY_SHAPES_2D_H
