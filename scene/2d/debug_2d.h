#pragma once

#include "core/engine.h"
#include "core/resource.h"
#include "scene/2d/node_2d.h"

class DebugCapture;

class Debug2D : public Node {
	GDCLASS(Debug2D, Node);

private:
	static Debug2D *singleton;

	bool enabled = true;

	Node2D *base = nullptr;
	ObjectID canvas_item; // Currently used item passed to draw commands.

	Dictionary draw_override;
	HashMap<String, Variant> default_value;
	bool antialiased = false;

	struct DrawCommand {
		enum Type {
			LINE,
			POLYLINE,
			POLYGON,
			REGION,
			RECTANGLE,
			CIRCLE,
			TEXT,
			TRANSFORM,
			CUSTOM,
		};
		ObjectID canvas_item;
		Type type;
		Vector<Variant> args;
	};
	Vector<DrawCommand> commands;

	Ref<DebugCapture> state;
	int capture_begin = 0;
	int capture_end = 0;

	bool update_queued = false;

protected:
	static void _bind_methods();

	void _on_canvas_item_draw(Object *p_item);
	void _push_command(DrawCommand &p_command);
	void _draw_command(const DrawCommand &p_command, CanvasItem *p_item);
	void _draw_shape(CanvasItem *p_item, const Vector<Point2> &p_points, const Vector2 &p_position, const Color &p_color, bool p_filled, float p_width);
	void _update_draw_commands();

	Variant _option_get_value(const String &p_option, const Variant &p_value);

public:
	static Debug2D *get_singleton() { return singleton; }

	void set_enabled(bool p_enabled);
	bool is_enabled() const { return enabled; }

	void set_canvas_item(Object *p_canvas_item);
	Object *get_canvas_item() const;
	Object *get_base() const { return base; }

	void draw(const StringName &p_method, const Array &p_args = Array()); // Custom.
	void draw_line(const Point2 &p_from, const Point2 &p_to, const Color &p_color = Color(1, 1, 1), float p_width = 1.0);
	void draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color = Color(1, 1, 1), float p_width = 1.0);
	void draw_polygon(const Vector<Point2> &p_polygon, const Color &p_color = Color(1, 1, 1), bool p_filled = true, float p_width = 1.0);
	void draw_region(const Rect2 &p_region, const Color &p_color = Color(1, 1, 1), bool p_filled = true, float p_width = 1.0);
	void draw_rectangle(const Vector2 &p_extents, const Vector2 &p_position = Vector2(), const Color &p_color = Color(1, 1, 1), bool p_filled = true, float p_width = 1.0);
	void draw_circle(real_t p_radius, const Vector2 &p_position = Vector2(), const Color &p_color = Color(1, 1, 1), bool p_filled = true, float p_width = 1.0);

	void draw_text(const String &p_text, const Vector2 &p_position = Vector2(), const Color &p_color = Color(1, 1, 1));

	void draw_set_transform(const Point2 &p_offset, float p_rotation = 0.0, const Size2 &p_scale = Size2(1, 1));
	void draw_set_transform_matrix(const Transform2D &p_matrix);

	void draw_set_color(const Color &p_color);
	void draw_set_filled(bool p_filled);
	void draw_set_line_width(real_t p_width);
	void draw_reset(const String &p_option = "");

	void capture();
	Ref<DebugCapture> get_capture() const { return state; }

	void update();
	void clear();

	Debug2D();
};

class DebugCapture : public Reference {
	GDCLASS(DebugCapture, Reference);

	friend class Debug2D;

protected:
	static void _bind_methods();

	Vector<int> snapshots;
	int snapshot = -1;
	bool accumulate = true;

public:
	void draw(int p_index);
	void draw_next();
	void draw_prev();

	int get_count() { return snapshots.size() / 2; }

	void set_accumulate(bool p_accumulate);
	bool is_accumulating() const { return accumulate; }

	void reset();
};
