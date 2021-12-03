#include "debug_2d.h"

#include "goost/core/math/geometry/2d/goost_geometry_2d.h"
#include "scene/scene_string_names.h"

Debug2D *Debug2D::singleton = nullptr;

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

void Debug2D::draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width) {
	ERR_FAIL_COND(p_polyline.size() < 2);

	DrawCommand c;
	c.type = DrawCommand::POLYLINE;
	c.args.push_back(p_polyline);
	c.args.push_back(_option_get_value("color", p_color));
	c.args.push_back(_option_get_value("line_width", p_width));
	_push_command(c);
}

void Debug2D::draw_circle(real_t p_radius, const Vector2 &p_position, const Color &p_color) {
	DrawCommand c;
	c.type = DrawCommand::CIRCLE;
	c.args.push_back(p_radius);
	c.args.push_back(_option_get_value("position", p_position));
	c.args.push_back(_option_get_value("color", p_color));
	_push_command(c);
}

void Debug2D::draw_set_color(const Color &p_color) {
	draw_override["color"] = p_color;
}

void Debug2D::draw_set_position(const Vector2 &p_position) {
	draw_override["position"] = p_position;
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
		draw_override["position"] = Variant();
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
	} else if (draw_override[p_option].get_type() == type) {
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
}

void Debug2D::_draw_command(const DrawCommand &p_command, CanvasItem *p_item) {
#ifdef DEBUG_ENABLED
	CanvasItem *item = Object::cast_to<CanvasItem>(ObjectDB::get_instance(p_command.canvas_item));
	if (!item) {
		return;
	}
	if (item != p_item) {
		return;
	}
	const DrawCommand &c = p_command; 

	switch (c.type) {
		case DrawCommand::POLYLINE: {
			item->draw_polyline(c.args[0], c.args[1], c.args[2], true);
		} break;
		case DrawCommand::CIRCLE: {
			const Vector<Vector2> &circle = GoostGeometry2D::circle(c.args[0]);
			item->draw_set_transform(c.args[1], 0, Size2(1, 1));
			item->draw_colored_polygon(circle, c.args[2], Vector<Point2>(), nullptr, nullptr, true);
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

void Debug2D::capture() {
	state->snapshots.push_back(capture_begin);
	state->snapshots.push_back(capture_end);

	capture_begin = capture_end;
	capture_end = commands.size();
}

void Debug2D::update() {
	for (int i = 0; i < commands.size(); ++i) {
		CanvasItem *item = Object::cast_to<CanvasItem>(ObjectDB::get_instance(commands[i].canvas_item));
		if (!item) {
			continue;
		}
		item->update();
	}
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
	}
	commands.clear();
	state->snapshots.clear();

	capture_begin = 0;
	capture_end = 0;

	set_canvas_item(base);

	draw_reset();
	update();
}

void Debug2D::_on_canvas_item_draw(Object *p_item) {
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

	default_value["color"] = GLOBAL_GET("debug/draw/default_color");
	default_value["position"] = Vector2();
	default_value["filled"] = true;
	default_value["line_width"] = 1.0;
}

void Debug2D::_bind_methods() {
	Color default_color = GLOBAL_GET("debug/draw/default_color");

	ClassDB::bind_method(D_METHOD("_on_canvas_item_draw", "item"), &Debug2D::_on_canvas_item_draw);

	ClassDB::bind_method(D_METHOD("set_canvas_item", "canvas_item"), &Debug2D::set_canvas_item);
	ClassDB::bind_method(D_METHOD("get_canvas_item"), &Debug2D::get_canvas_item);
	ClassDB::bind_method(D_METHOD("get_base"), &Debug2D::get_base);

	ClassDB::bind_method(D_METHOD("draw", "method", "args"), &Debug2D::draw, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("draw_polyline", "polyline", "color", "width"), &Debug2D::draw_polyline, default_color, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("draw_circle", "radius", "position", "color"), &Debug2D::draw_circle, DEFVAL(Vector2()), default_color);

	ClassDB::bind_method(D_METHOD("draw_set_color", "color"), &Debug2D::draw_set_color);
	ClassDB::bind_method(D_METHOD("draw_set_position", "position"), &Debug2D::draw_set_position);
	ClassDB::bind_method(D_METHOD("draw_set_filled", "filled"), &Debug2D::draw_set_filled);
	ClassDB::bind_method(D_METHOD("draw_set_line_width", "width"), &Debug2D::draw_set_line_width);
	ClassDB::bind_method(D_METHOD("draw_reset", "option"), &Debug2D::draw_reset, DEFVAL(""));

	ClassDB::bind_method(D_METHOD("get_capture"), &Debug2D::get_capture);
	ClassDB::bind_method(D_METHOD("capture"), &Debug2D::capture);

	ClassDB::bind_method(D_METHOD("update"), &Debug2D::update);
	ClassDB::bind_method(D_METHOD("clear"), &Debug2D::clear);

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
