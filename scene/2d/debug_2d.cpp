#include "debug_2d.h"

#include "goost/core/math/geometry/2d/goost_geometry_2d.h"

#include "scene/resources/theme.h"
#include "scene/scene_string_names.h"

#include "core/method_bind_ext.gen.inc"

Debug2D *Debug2D::singleton = nullptr;

void Debug2D::set_enabled(bool p_enabled) {
	enabled = p_enabled;
	update();
}

void Debug2D::set_canvas_item(Object *p_canvas_item) {
	ERR_FAIL_NULL_MSG(p_canvas_item, "Invalid object");

	CanvasItem *item = nullptr;
	if (p_canvas_item) {
		item = Object::cast_to<CanvasItem>(p_canvas_item);
	}
	ERR_FAIL_NULL_MSG(item, "Object does inherit CanvasItem");

	canvas_item = item->get_instance_id();

	if (!item->is_connected(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw")) {
		item->connect(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw", varray(item));
	}
}

Object *Debug2D::get_canvas_item() const {
	return ObjectDB::get_instance(canvas_item);
}

void Debug2D::draw(const StringName &p_method, const Array &p_args) {
	DrawCommand c;
	c.type = DrawCommand::CUSTOM;
	c.args.push_back(p_method);
	c.args.push_back(p_args);
	_push_command(c);
}

void Debug2D::draw_line(const Point2 &p_from, const Point2 &p_to, const Color &p_color, float p_width) {
	DrawCommand c;
	c.type = DrawCommand::LINE;
	c.args.push_back(p_from);
	c.args.push_back(p_to);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_arrow(const Point2 &p_from, const Point2 &p_to, const Color &p_color, float p_width, const Vector2 &p_tip_size, float p_tip_offset) {
	ERR_FAIL_COND(p_tip_size.x <= 0);
	ERR_FAIL_COND(p_tip_size.y <= 0);

	DrawCommand c;
	c.type = DrawCommand::ARROW;
	c.args.push_back(p_from);
	c.args.push_back(p_to);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("line_width", p_width));
	c.args.push_back(p_tip_size);
	c.args.push_back(p_tip_offset);
	_push_command(c);
}

void Debug2D::draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width) {
	ERR_FAIL_COND(p_polyline.size() < 2);

	DrawCommand c;
	c.type = DrawCommand::POLYLINE;
	c.args.push_back(p_polyline);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_polygon(const Vector<Point2> &p_polygon, const Color &p_color, bool p_filled, float p_width) {
	ERR_FAIL_COND(p_polygon.size() < 3);

	DrawCommand c;
	c.type = DrawCommand::POLYGON;
	c.args.push_back(p_polygon);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("filled", p_filled));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_region(const Rect2 &p_region, const Color &p_color, bool p_filled, float p_width) {
	DrawCommand c;
	c.type = DrawCommand::REGION;
	c.args.push_back(p_region);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("filled", p_filled));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_rectangle(const Vector2 &p_extents, const Vector2 &p_position, const Color &p_color, bool p_filled, float p_width) {
	DrawCommand c;
	c.type = DrawCommand::RECTANGLE;
	c.args.push_back(p_extents);
	c.args.push_back(p_position);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("filled", p_filled));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_circle(real_t p_radius, const Vector2 &p_position, const Color &p_color, bool p_filled, float p_width) {
	DrawCommand c;
	c.type = DrawCommand::CIRCLE;
	c.args.push_back(p_radius);
	c.args.push_back(p_position);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("filled", p_filled));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_text(const String &p_text, const Vector2 &p_position, const Color &p_color) {
	DrawCommand c;
	c.type = DrawCommand::TEXT;
	c.args.push_back(p_text);
	c.args.push_back(p_position);
	c.args.push_back(_option_get_value("color", p_color));
	_push_command(c);
}

void Debug2D::draw_set_transform(const Point2 &p_offset, float p_rotation, const Size2 &p_scale) {
	Transform2D matrix(p_rotation, p_offset);
	matrix.scale_basis(p_scale);
	draw_set_transform_matrix(matrix);
}

void Debug2D::draw_set_transform_matrix(const Transform2D &p_matrix) {
	DrawCommand c;
	c.type = DrawCommand::TRANSFORM;
	c.args.push_back(p_matrix);
	_push_command(c);
}

void Debug2D::draw_set_color(const Color &p_color) {
	draw_override["color"] = p_color;
}

void Debug2D::draw_set_filled(bool p_filled) {
	draw_override["filled"] = p_filled;
}

void Debug2D::draw_set_line_width(real_t p_width) {
	draw_override["line_width"] = p_width;
}

void Debug2D::draw_reset(const String &p_option) {
	if (p_option.empty()) {
		draw_override["color"] = Variant();
		draw_override["filled"] = Variant();
		draw_override["line_width"] = Variant();
	} else {
		ERR_FAIL_COND_MSG(!draw_override.has(p_option), "Draw option does not exist");
		draw_override[p_option] = Variant();
	}
}

Variant Debug2D::_option_get_value(const String &p_option, const Variant &p_value) {
	Variant ret;

	Variant def_value = default_value[p_option];
	Variant::Type type = def_value.get_type();

	if (p_value != def_value) {
		ret = p_value;
	} else if (draw_override[p_option].get_type() == type) { // Not NIL.
		ret = draw_override[p_option];
	} else {
		ret = p_value;
	}
	return ret;
}

void Debug2D::_push_command(DrawCommand &p_command) {
	p_command.canvas_item = canvas_item;
	commands.push_back(p_command);
	capture_end += 1;
	update();
}

void Debug2D::_draw_command(const DrawCommand &p_command, CanvasItem *p_item) {
#ifdef DEBUG_ENABLED
	if (!enabled) {
		return;
	}
	CanvasItem *item = Object::cast_to<CanvasItem>(ObjectDB::get_instance(p_command.canvas_item));
	if (!item) {
		return;
	}
	if (item != p_item) {
		return;
	}
	const DrawCommand &c = p_command;

	switch (c.type) {
		case DrawCommand::LINE: {
			item->draw_line(c.args[0], c.args[1], c.args[2], c.args[3], antialiased);
		} break;
		case DrawCommand::ARROW: {
			Vector2 from = c.args[0];
			Vector2 to = c.args[1];
			Color color = c.args[2];
			float line_width = c.args[3];
			Vector2 tip_size = c.args[4];
			float tip_offset = c.args[5];

			Vector2 vector = to - from;
			real_t half_length = vector.length() * 0.5;
			tip_size = Vector2(MAX(tip_size.x, line_width), tip_size.y);

			if (half_length < tip_size.y) {
				float ratio = half_length / tip_size.y;
				tip_size.y *= ratio;
				tip_size.x = MAX(tip_size.x * ratio, line_width);
			}
			Transform2D trans(vector.angle(), to);
			Vector2 dest;
			if (tip_offset <= 0.0) {
				dest = trans.xform(Vector2(-tip_size.y, 0));
			} else {
				dest = to;
			}
			item->draw_line(from, dest, color, line_width, antialiased);

			Vector<Point2> tip;
			Vector2 shift = Vector2(-vector.length() * tip_offset, 0);
			tip.push_back(trans.xform(shift));
			tip.push_back(trans.xform(Vector2(-tip_size.y + shift.x, tip_size.x * 0.5)));
			tip.push_back(trans.xform(Vector2(-tip_size.y + shift.x, -tip_size.x * 0.5)));

			// Could use `draw_primitive()`, but it doesn't support antialiasing.
			item->draw_colored_polygon(tip, color, Vector<Point2>(), nullptr, nullptr, antialiased);
		} break;
		case DrawCommand::POLYLINE: {
			item->draw_polyline(c.args[0], c.args[1], c.args[2], antialiased);
		} break;
		case DrawCommand::POLYGON: {
			// Godot's `draw_polygon()` is not as robust as it could be.
			// The following works better for rendering degenerate and
			// self-intersecting polygons.
			Vector<Vector2> polygon = c.args[0];
			Color color = c.args[1];
			bool filled = c.args[2];
			float width = c.args[3];

			if (filled) {
				const Vector<Vector<Point2>> &triangles = GoostGeometry2D::triangulate_polygon(polygon);
				if (triangles.empty()) {
					break;
				}
				Vector<Point2> vertices;
				for (int i = 0; i < triangles.size(); ++i) {
					const Vector<Point2> &part = triangles[i];
					for (int j = 0; j < part.size(); ++j) {
						vertices.push_back(part[j]);
					}
				}
				const int indices_count = triangles.size() * 3;
				Vector<int> indices;
				for (int i = 0; i < indices_count; ++i) {
					indices.push_back(i);
				}
				Vector<Color> colors;
				colors.push_back(color);

				VS::get_singleton()->canvas_item_add_triangle_array(
						item->get_canvas_item(), indices, vertices, colors, Vector<Point2>(),
						Vector<int>(), Vector<float>(), RID(), -1, RID(), antialiased);
			} else {
				polygon.push_back(polygon[0]); // Close it.
				item->draw_polyline(polygon, color, width, antialiased);
			}
		} break;
		case DrawCommand::REGION: {
			bool filled = c.args[2];
			// Get rid of annoying warnings, because that's an implementation detail.
			if (filled) {
				item->draw_rect(c.args[0], c.args[1], true);
			} else {
				item->draw_rect(c.args[0], c.args[1], false, c.args[3], antialiased);
			}
		} break;
		case DrawCommand::RECTANGLE: {
			Vector2 extents = c.args[0];
			Vector2 position = c.args[1];
			Color color = c.args[2];
			bool filled = c.args[3];

			Rect2 rect = Rect2(position - extents, extents * 2);
			if (filled) {
				item->draw_rect(rect, color, true);
			} else {
				item->draw_rect(rect, color, false, c.args[4], antialiased);
			}
		} break;
		case DrawCommand::CIRCLE: {
			// Godot's `draw_circle()` produces fixed number of vertices.
			// The following works better with large circles.
			const Vector<Vector2> &circle = GoostGeometry2D::circle(c.args[0]);
			_draw_shape(item, circle, c.args[1], c.args[2], c.args[3], c.args[4]);
		} break;
		case DrawCommand::TEXT: {
			const Ref<Font> &font = Theme::get_default()->get_font("font", "Control");
			String text = c.args[0];
			Vector2 position = c.args[1];
			Color color = c.args[2];
			item->draw_string(font, position, text, color);
		} break;
		case DrawCommand::TRANSFORM: {
			item->draw_set_transform_matrix(c.args[0]);
		} break;
		case DrawCommand::CUSTOM: {
			String method = c.args[0];
			if (!item->has_method(method)) {
				method = "draw_" + method; // Try this one.
			}
			item->callv(method, c.args[1]);
		} break;
	}
#endif
}

void Debug2D::_draw_shape(CanvasItem *p_item, const Vector<Point2> &p_points, const Vector2 &p_position, const Color &p_color, bool p_filled, float p_width) {
	Vector<Vector2> points;
	for (int i = 0; i < p_points.size(); ++i) {
		points.push_back(p_points[i] + p_position);
	}
	if (p_filled) {
		p_item->draw_colored_polygon(points, p_color, Vector<Point2>(), nullptr, nullptr, antialiased);
	} else {
		points.push_back(points[0]);
		p_item->draw_polyline(points, p_color, p_width, antialiased);
	}
}

void Debug2D::capture() {
	state->snapshots.push_back(capture_begin);
	state->snapshots.push_back(capture_end);

	capture_begin = capture_end;
	capture_end = commands.size();
}

void Debug2D::update() {
	if (!is_inside_tree()) {
		return;
	}
	if (update_queued) {
		return;
	}
	update_queued = true;
	call_deferred("_update_draw_commands");
}

void Debug2D::_update_draw_commands() {
	for (int i = 0; i < commands.size(); ++i) {
		CanvasItem *item = Object::cast_to<CanvasItem>(ObjectDB::get_instance(commands[i].canvas_item));
		if (!item) {
			continue;
		}
		item->update();
	}
	update_queued = false;
}

void Debug2D::clear() {
	for (int i = 0; i < commands.size(); ++i) {
		CanvasItem *item = Object::cast_to<CanvasItem>(ObjectDB::get_instance(commands[i].canvas_item));
		if (!item) {
			continue;
		}
		if (item->is_connected(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw")) {
			item->disconnect(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw");
		}
		item->update();
	}
	commands.clear();
	state->snapshots.clear();

	capture_begin = 0;
	capture_end = 0;

	set_canvas_item(base);

	draw_reset();
}

void Debug2D::_on_canvas_item_draw(Object *p_item) {
#ifdef DEBUG_ENABLED
	if (!enabled) {
		return;
	}
	CanvasItem *item = Object::cast_to<CanvasItem>(p_item);
	ERR_FAIL_NULL(item);

	int snapshot_idx = 0;
	int begin = capture_begin;
	int end = capture_end;

	for (int i = 0; i < state->snapshots.size(); i += 2) {
		begin = state->snapshots[i];
		end = state->snapshots[i + 1];

		if (!state->accumulate) {
			if (snapshot_idx != state->snapshot) {
				// Do not accumulate drawings on top of each command.
				++snapshot_idx;
				continue;
			}
		}
		for (int j = begin; j < end; ++j) {
			_draw_command(commands[j], item);
		}
		++snapshot_idx;
		if (state->snapshot >= 0 && snapshot_idx > state->snapshot) {
			// Stop drawing at this point.
			break;
		}
	}
	// Process rest of the commands that were not explicitly captured.
	// These type of commands will be drawn regardless.
	if (state->snapshots.empty()) {
		for (int j = 0; j < commands.size(); ++j) {
			_draw_command(commands[j], item);
		}
	} else {
		begin = state->snapshots[state->snapshots.size() - 1];
		end = commands.size();
		for (int j = begin; j < end; ++j) {
			_draw_command(commands[j], item);
		}
	}
#endif
}

Debug2D::Debug2D() {
	ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
	singleton = this;
	state.instance();

	base = memnew(Node2D);
	set_canvas_item(base);

	base->set_name("Canvas");
	add_child(base);

	draw_reset();

	default_value["color"] = GLOBAL_GET("debug/draw/2d/color");
	default_value["filled"] = GLOBAL_GET("debug/draw/2d/filled");
	default_value["line_width"] = GLOBAL_GET("debug/draw/2d/line_width");

	antialiased = GLOBAL_GET("debug/draw/2d/antialiased");
}

void Debug2D::_bind_methods() {
	Color color = GLOBAL_GET("debug/draw/2d/color");
	bool filled = GLOBAL_GET("debug/draw/2d/filled");
	float line_width = GLOBAL_GET("debug/draw/2d/line_width");

	ClassDB::bind_method(D_METHOD("_on_canvas_item_draw", "item"), &Debug2D::_on_canvas_item_draw);
	ClassDB::bind_method(D_METHOD("_update_draw_commands"), &Debug2D::_update_draw_commands);

	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &Debug2D::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &Debug2D::is_enabled);

	ClassDB::bind_method(D_METHOD("set_canvas_item", "canvas_item"), &Debug2D::set_canvas_item);
	ClassDB::bind_method(D_METHOD("get_canvas_item"), &Debug2D::get_canvas_item);
	ClassDB::bind_method(D_METHOD("get_base"), &Debug2D::get_base);

	ClassDB::bind_method(D_METHOD("draw", "method", "args"), &Debug2D::draw, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("draw_line", "from", "to", "color", "width"), &Debug2D::draw_line, color, line_width);
	ClassDB::bind_method(D_METHOD("draw_arrow", "from", "to", "color", "width", "tip_size", "tip_offset"), &Debug2D::draw_arrow, color, line_width, Vector2(8, 8), 0.0);

	ClassDB::bind_method(D_METHOD("draw_polyline", "polyline", "color", "width"), &Debug2D::draw_polyline, color, line_width);
	ClassDB::bind_method(D_METHOD("draw_polygon", "polygon", "color", "filled", "width"), &Debug2D::draw_polygon, color, filled, line_width);

	ClassDB::bind_method(D_METHOD("draw_region", "region", "color", "filled", "width"), &Debug2D::draw_region, color, filled, line_width);

	ClassDB::bind_method(D_METHOD("draw_rectangle", "extents", "position", "color", "filled", "width"), &Debug2D::draw_rectangle, color, filled, line_width);
	ClassDB::bind_method(D_METHOD("draw_circle", "radius", "position", "color", "filled", "width"), &Debug2D::draw_circle, DEFVAL(Vector2()), color, filled, line_width);

	ClassDB::bind_method(D_METHOD("draw_text", "text", "position", "color"), &Debug2D::draw_text, DEFVAL(Vector2()), Color(1, 1, 1)); // White by default!

	ClassDB::bind_method(D_METHOD("draw_set_transform", "position", "rotation", "scale"), &Debug2D::draw_set_transform, DEFVAL(0), DEFVAL(Size2(1, 1)));
	ClassDB::bind_method(D_METHOD("draw_set_transform_matrix", "matrix"), &Debug2D::draw_set_transform_matrix);

	ClassDB::bind_method(D_METHOD("draw_set_color", "color"), &Debug2D::draw_set_color);
	ClassDB::bind_method(D_METHOD("draw_set_filled", "filled"), &Debug2D::draw_set_filled);
	ClassDB::bind_method(D_METHOD("draw_set_line_width", "width"), &Debug2D::draw_set_line_width);
	ClassDB::bind_method(D_METHOD("draw_reset", "option"), &Debug2D::draw_reset, DEFVAL(""));

	ClassDB::bind_method(D_METHOD("get_capture"), &Debug2D::get_capture);
	ClassDB::bind_method(D_METHOD("capture"), &Debug2D::capture);

	ClassDB::bind_method(D_METHOD("update"), &Debug2D::update);
	ClassDB::bind_method(D_METHOD("clear"), &Debug2D::clear);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "canvas_item"), "set_canvas_item", "get_canvas_item");
}

// DebugCapture

void DebugCapture::draw(int p_index) {
	ERR_FAIL_INDEX(p_index, get_count());
	snapshot = p_index;
	Debug2D::get_singleton()->update();
}

void DebugCapture::draw_next() {
	snapshot = CLAMP(snapshot + 1, 0, get_count() - 1);
	Debug2D::get_singleton()->update();
}

void DebugCapture::draw_prev() {
	snapshot = CLAMP(snapshot - 1, 0, get_count() - 1);
	Debug2D::get_singleton()->update();
}

void DebugCapture::set_accumulate(bool p_accumulate) {
	accumulate = p_accumulate;
	Debug2D::get_singleton()->update();
}

void DebugCapture::reset() {
	snapshot = -1;
	Debug2D::get_singleton()->update();
}

void DebugCapture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("draw", "index"), &DebugCapture::draw);
	ClassDB::bind_method(D_METHOD("draw_next"), &DebugCapture::draw_next);
	ClassDB::bind_method(D_METHOD("draw_prev"), &DebugCapture::draw_prev);

	ClassDB::bind_method(D_METHOD("get_count"), &DebugCapture::get_count);

	ClassDB::bind_method(D_METHOD("reset"), &DebugCapture::reset);

	ClassDB::bind_method(D_METHOD("set_accumulate", "accumulate"), &DebugCapture::set_accumulate);
	ClassDB::bind_method(D_METHOD("is_accumulating"), &DebugCapture::is_accumulating);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "accumulate"), "set_accumulate", "is_accumulating");
}
