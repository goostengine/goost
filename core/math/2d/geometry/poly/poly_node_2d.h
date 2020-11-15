#ifndef GOOST_POLY_NODE_2D_H
#define GOOST_POLY_NODE_2D_H

#include "scene/2d/node_2d.h"

class PolyNode2D : public Node2D {
	GDCLASS(PolyNode2D, Node2D);

public:
	enum Operation {
		OP_NONE,
		OP_UNION,
		OP_DIFFERENCE,
		OP_INTERSECTION,
		OP_XOR,
	};

private:
	Vector<Point2> points;
	Operation operation = OP_NONE;
	Color color = Color(1, 1, 1, 1);
	bool open = false; // closed = polygon, open = polyline
	bool filled = true; // for polygons
	real_t width = 1.0;

	PolyNode2D *parent = nullptr;
	bool update_queued = false;
	void _update_outlines();

protected:
	void _notification(int p_what);
	virtual void _validate_property(PropertyInfo &property) const;
	static void _bind_methods();

	void _draw();
	void _queue_update();

	Vector<Vector<Point2>> outlines;
	Vector<Vector<Point2>> decomp;

	virtual Vector<Vector<Point2>> _build_outlines();
	Vector<Vector<Point2>> _get_outlines();
	void _collect_outlines(Vector<Vector<Point2>> *r_closed, Vector<Vector<Point2>> *r_open = nullptr);

public:
	void set_points(const Vector<Point2> &p_points);
	Vector<Point2> get_points() const { return points; }
	Vector<Point2> get_points_transformed();

	void set_operation(Operation p_operation);
	Operation get_operation() const { return operation; }

	void set_color(const Color &p_color);
	Color get_color() const { return color; }

	void set_open(bool p_open);
	bool is_open() const { return open; }

	void set_filled(bool p_filled);
	bool is_filled() const { return filled; }

	void set_width(real_t p_width);
	real_t get_width() const { return width; }

	PolyNode2D *new_child(const Vector<Point2> &p_points);

	bool is_hole() const;
	bool is_root() const { return !parent; }

	void make_from_outlines(const Array &p_outlines);
	Array collect_polygons();
	Array collect_polylines();
	Array find_objects();
	Array separate_objects(Node *p_new_parent = nullptr, bool p_keep_transform = true);

	void clear();

	PolyNode2D();
};

VARIANT_ENUM_CAST(PolyNode2D::Operation);

template <>
struct VariantCaster<PolyNode2D *> {
	static _FORCE_INLINE_ PolyNode2D *cast(const Variant &p_variant) {
		return (PolyNode2D *)p_variant.operator Object *();
	}
};

#endif // GOOST_POLY_NODE_2D_H
