#include "visual_shape_2d.h"

#include "core/core_string_names.h"
#include "core/engine.h"

void VisualShape2D::set_shape(const Ref<Shape2D> &p_shape) {
	shape = p_shape;
	if (p_shape.is_valid()) {
		shape->connect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	update();
}

Ref<Shape2D> VisualShape2D::get_shape() const {
	return shape;
}

void VisualShape2D::set_color(const Color &p_color) {
	color = p_color;
	update();
}

Color VisualShape2D::get_color() const {
	return color;
}

void VisualShape2D::set_debug_use_default_color(bool p_debug_use_default_color) {
	debug_use_default_color = p_debug_use_default_color;
	update();
}

bool VisualShape2D::is_using_debug_default_color() const {
	return debug_use_default_color;
}

void VisualShape2D::set_debug_sync_visible_collision_shapes(bool p_debug_sync_visible_collision_shapes) {
	debug_sync_visible_collision_shapes = p_debug_sync_visible_collision_shapes;
	update();
}

bool VisualShape2D::is_debug_sync_visible_collision_shapes() const {
	return debug_sync_visible_collision_shapes;
}

void VisualShape2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (shape.is_null()) {
				break;
			}
			Color draw_color = color;
#ifdef TOOLS_ENABLED
			if (debug_use_default_color) {
				draw_color = get_tree()->get_debug_collisions_color();
			}
			if (debug_sync_visible_collision_shapes && !Engine::get_singleton()->is_editor_hint()) {
				if (!get_tree()->is_debugging_collisions_hint()) {
					break;
				}
			}
#endif
			shape->draw(get_canvas_item(), draw_color);
		} break;
	}
}

String VisualShape2D::get_configuration_warning() const {
	String warning = Node2D::get_configuration_warning();

	if (shape.is_null()) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("Shape2D is required for this node to be drawn.");
	}

	return warning;
}

void VisualShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shape", "shape"), &VisualShape2D::set_shape);
	ClassDB::bind_method(D_METHOD("get_shape"), &VisualShape2D::get_shape);

	ClassDB::bind_method(D_METHOD("set_color", "color"), &VisualShape2D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &VisualShape2D::get_color);

	ClassDB::bind_method(D_METHOD("set_debug_use_default_color", "enable"), &VisualShape2D::set_debug_use_default_color);
	ClassDB::bind_method(D_METHOD("is_using_debug_default_color"), &VisualShape2D::is_using_debug_default_color);

	ClassDB::bind_method(D_METHOD("set_debug_sync_visible_collision_shapes", "debug_sync_visible_collision_shapes"), &VisualShape2D::set_debug_sync_visible_collision_shapes);
	ClassDB::bind_method(D_METHOD("is_debug_sync_visible_collision_shapes"), &VisualShape2D::is_debug_sync_visible_collision_shapes);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_shape", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_GROUP("Debug", "debug_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_use_default_color"), "set_debug_use_default_color", "is_using_debug_default_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_sync_visible_collision_shapes"), "set_debug_sync_visible_collision_shapes", "is_debug_sync_visible_collision_shapes");
}
