#ifndef GOOST_POLY_NODE_2D_H
#define GOOST_POLY_NODE_2D_H

#include "scene/2d/node_2d.h"

class PolyNode2D : public Node2D {
	GDCLASS(PolyNode2D, Node2D);

private:
	Vector<Point2> points;
	bool open = false; // closed = polygon, open = polyline

protected:
	void _notification(int p_what);
	virtual void _validate_property(PropertyInfo &property) const;
	static void _bind_methods();

	void _queue_update();
	void _collect_outlines(Vector<Vector<Point2>> *r_closed, Vector<Vector<Point2>> *r_open = nullptr);

public:
	void set_points(const Vector<Point2> &p_points);
	Vector<Point2> get_points() const { return points; }

	void set_open(bool p_open);
	bool is_open() const { return open; }

	PolyNode2D *new_child(const Vector<Point2> &p_points);

	bool is_hole() const;
	bool is_root() const;

	void create_from_polygons(const Array &p_polygons);
	void clear();

	String get_configuration_warning() const;

	PolyNode2D();
};

template <>
struct VariantCaster<PolyNode2D *> {
	static _FORCE_INLINE_ PolyNode2D *cast(const Variant &p_variant) {
		return (PolyNode2D *)p_variant.operator Object *();
	}
};

#endif // GOOST_POLY_NODE_2D_H
