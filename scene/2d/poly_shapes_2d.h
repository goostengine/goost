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
		_queue_update();
	}
};

#endif // GOOST_POLY_SHAPES_2D_H
