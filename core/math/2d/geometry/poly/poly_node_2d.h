#ifndef GOOST_POLY_NODE_2D_H
#define GOOST_POLY_NODE_2D_H

#include "scene/2d/node_2d.h"

class PolyNode2D : public Node2D {
	GDCLASS(PolyNode2D, Node2D);

private:
	Vector<Point2> points;

protected:
	static void _bind_methods();

public:
	void set_points(const Vector<Point2> &p_points) { points = p_points; }
	Vector<Point2> get_points() const { return points; }

	PolyNode2D *new_child(const Vector<Point2> &p_points);
	bool is_hole() const;
	void clear();
};

template <>
struct VariantCaster<PolyNode2D *> {
	static _FORCE_INLINE_ PolyNode2D *cast(const Variant &p_variant) {
		return (PolyNode2D *)p_variant.operator Object *();
	}
};

#endif // GOOST_POLY_NODE_2D_H
