#ifndef POLY_SHAPE_2D_H
#define POLY_SHAPE_2D_H

#include "goost/core/math/2d/geometry/poly/poly_node_2d.h"

class PolyShape2D : public Node2D {
	GDCLASS(PolyShape2D, Node2D);

public:
	enum BuildMode {
		BUILD_TRIANGLES,
		BUILD_CONVEX,
		BUILD_SEGMENTS,
	};

private:
	Vector<Vector<Point2>> _collect_outlines();

protected:
	Vector<Vector<Point2>> shapes;
	bool update_queued = false;

	BuildMode build_mode = BUILD_TRIANGLES;
	Rect2 rect = Rect2(-10, -10, 20, 20);
	PolyNode2D *child = nullptr;

	virtual Vector<Vector<Point2>> _build_shapes();
	virtual void _apply_shapes(){};
	void _update_shapes();
	void _queue_update();

	virtual void add_child_notify(Node *p_child);
	virtual void remove_child_notify(Node *p_child);

	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_build_mode(BuildMode p_mode);
	BuildMode get_build_mode() const { return build_mode; }

	void update_shapes();
	Array get_shapes_array();

	virtual String get_configuration_warning() const;
};

VARIANT_ENUM_CAST(PolyShape2D::BuildMode);

#endif // POLY_SHAPE_2D_H
