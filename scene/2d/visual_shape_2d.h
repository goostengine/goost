#ifndef GOOST_VISUAL_SHAPE_2D_H
#define GOOST_VISUAL_SHAPE_2D_H

#include "scene/2d/node_2d.h"
#include "scene/resources/concave_polygon_shape_2d.h"
#include "scene/resources/convex_polygon_shape_2d.h"
#include "scene/resources/shape_2d.h"

class VisualShape2D : public Node2D {
	GDCLASS(VisualShape2D, Node2D);

	Ref<Shape2D> shape;

	Ref<Shape2D> parent_shape;
	bool use_parent_shape = false;

	// Cache polygon-based parent geometry with this instance.
	Ref<ConvexPolygonShape2D> polygon_shape;

	Color color = Color(1, 1, 1, 1);

	bool debug_use_default_color = false;
	bool debug_sync_visible_collision_shapes = false;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_shape() const;

	void set_use_parent_shape(bool p_use_parent_shape);
	bool is_using_parent_shape() const;
	// Returns `true` if parent shape is changed.
	bool update_parent_shape();

	void set_color(const Color &p_color);
	Color get_color() const;

	void set_debug_use_default_color(bool p_debug_use_default_color);
	bool is_using_debug_default_color() const;

	void set_debug_sync_visible_collision_shapes(bool p_debug_sync_visible_collision_shapes);
	bool is_debug_sync_visible_collision_shapes() const;

	String get_configuration_warning() const;

	VisualShape2D(){};
};

#endif // GOOST_VISUAL_SHAPE_2D_H
