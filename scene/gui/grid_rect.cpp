#include "grid_rect.h"

#include "core/engine.h"
#include "core/variant_parser.h"

#include "core/method_bind_ext.gen.inc"

void GridRect::set_cell_size(const Vector2 &p_size) {
	cell_size = p_size;
	update();
}

void GridRect::set_cell_origin(CellOrigin p_origin) {
	cell_origin = p_origin;
	update();
}

void GridRect::set_cell_line_width(float p_width) {
	cell_line_width = MAX(0.0f, p_width);
	update();
}

void GridRect::set_origin_offset(Vector2 p_offset) {
	origin_offset = p_offset;
	update();
}

void GridRect::set_origin_scale(Vector2 p_scale) {
	origin_scale = p_scale; // Can be negative.
	update();
}

void GridRect::set_origin_centered(bool p_centered) {
	origin_centered = p_centered;
	update();
}

void GridRect::set_origin_axes_visible(bool p_visible) {
	origin_axes_visible = p_visible;
	_change_notify();
	update();
}

void GridRect::set_origin_axes_line_width(float p_width) {
	origin_axes_line_width = MAX(0, p_width);
	update();
}

void GridRect::set_subdivisions_horizontal(int p_count) {
	subdivisions_horizontal = MAX(0, p_count);
	update();
}

void GridRect::set_subdivisions_vertical(int p_count) {
	subdivisions_vertical = MAX(0, p_count);
	update();
}

void GridRect::set_subdivisions_line_width(float p_width) {
	subdivisions_line_width = MAX(0.0f, p_width);
	update();
}

void GridRect::set_metadata_show_tooltip(bool p_enabled) {
	metadata_show_tooltip = p_enabled;
	update();
}

Vector2 GridRect::_get_final_offset(CellOrigin p_cell_origin) {
	Vector2 ofs = origin_offset;
	if (origin_centered) {
		ofs += -get_size() / 2;
	}
	if (p_cell_origin == CELL_ORIGIN_CENTER) {
		ofs -= (cell_size * origin_scale) / 2;
	}
	return ofs;
}

Vector2 GridRect::view_to_point(const Vector2 &p_position) {
	const Vector2 &ofs = _get_final_offset(cell_origin);
	return (p_position + ofs) / (cell_size * origin_scale);
}

Vector2 GridRect::view_to_point_snapped(const Vector2 &p_position) {
	const Vector2 &ofs = _get_final_offset(cell_origin);
	return ((p_position + ofs) / (cell_size * origin_scale)).snapped(Vector2(1, 1));
}

Vector2 GridRect::point_to_view(const Vector2 &p_point) {
	const Vector2 &ofs = _get_final_offset(cell_origin);
	return p_point * cell_size * origin_scale - ofs;
}

void GridRect::set_metadata(const Vector2 &p_point, const Variant &p_metadata) {
	metadata.insert(p_point, p_metadata);
}

Variant GridRect::get_metadata(const Vector2 &p_point) {
	Variant ret;
	if (metadata.has(p_point)) {
		ret = metadata[p_point];
	}
	return ret;
}

void GridRect::_gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		_point_snapped = view_to_point_snapped(mm->get_position()); // For tooltip.
	}
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_pressed() && mb->get_button_index() == BUTTON_LEFT) {
			const Vector2 &point = view_to_point(mb->get_position());
			const Vector2 &snapped = view_to_point_snapped(mb->get_position());
			emit_signal("point_clicked", point, snapped, mb->get_position());
		}
	}
}

bool GridRect::_draw_line(const Vector2 &p_from, const Vector2 &p_to, const Color &p_color, float p_width, const Dictionary &p_line) {
	auto script = get_script_instance();
	if (script && script->has_method("_draw_line")) {
		return script->call("_draw_line", p_from, p_to, p_color, p_width, p_line);
	}
	return true; // Draw everything by default.
}

void GridRect::_draw_grid_vertical(int from, int to, const Vector2 &p_ofs, Line p_type) {
	for (int i = from; i < to + 1; ++i) {
		Color color;
		float width;

		if (p_type == LINE_MINOR) {
			width = cell_line_width;
			color = _minor_color;
		} else if (p_type == LINE_MAJOR) {
			bool should_draw = subdivisions_vertical != 0 && (ABS(i) % subdivisions_vertical == 0);
			if (!should_draw) {
				continue;
			}
			width = subdivisions_line_width;
			color = _major_color;
		} else if (p_type == LINE_AXIS) {
			bool should_draw = origin_axes_visible && i == 0;
			if (!should_draw) {
				continue;
			}
			width = origin_axes_line_width;
			color = _axis_y_color;
		}

		if (width <= 0.0f) {
			continue; // Has an effect of hiding lines.
		}

		Vector2 scale = origin_scale.abs();
		real_t base_ofs = i * cell_size.x * scale.x - p_ofs.x * scale.x;
		Vector2 from_pos = Vector2(base_ofs, 0);
		Vector2 to_pos = Vector2(base_ofs, get_size().y);

		current_line["step"] = i;
		current_line["axis"] = AXIS_Y;
		current_line["type"] = p_type;

		bool to_draw = _draw_line(from_pos, to_pos, color, width, current_line);
		if (to_draw) {
			draw_line(from_pos, to_pos, color, width);
		}
	}
}

void GridRect::_draw_grid_horizontal(int from, int to, const Vector2 &p_ofs, Line p_type) {
	for (int i = from; i < to + 1; ++i) {
		Color color;
		float width;

		if (p_type == LINE_MINOR) {
			width = cell_line_width;
			color = _minor_color;
		} else if (p_type == LINE_MAJOR) {
			bool should_draw = subdivisions_horizontal != 0 && (ABS(i) % subdivisions_horizontal == 0);
			if (!should_draw) {
				continue;
			}
			width = subdivisions_line_width;
			color = _major_color;
		} else if (p_type == LINE_AXIS) {
			bool should_draw = origin_axes_visible && i == 0;
			if (!should_draw) {
				continue;
			}
			width = origin_axes_line_width;
			color = _axis_x_color;
		}

		if (width <= 0.0f) {
			continue; // Has an effect of hiding lines.
		}

		Vector2 scale = origin_scale.abs();
		real_t base_ofs = i * cell_size.y * scale.y - p_ofs.y * scale.y;
		Vector2 from_pos = Vector2(0, base_ofs);
		Vector2 to_pos = Vector2(get_size().x, base_ofs);

		current_line["step"] = i;
		current_line["axis"] = AXIS_X;
		current_line["type"] = p_type;

		bool to_draw = _draw_line(from_pos, to_pos, color, width, current_line);
		if (to_draw) {
			draw_line(from_pos, to_pos, color, width);
		}
	}
}

void GridRect::_update_colors() {
	if (has_color_override("line_minor")) {
		_minor_color = get_color("line_minor");
	} else if (has_color("grid_minor", "GraphEdit")) {
		// Reuse grid colors from GraphEdit.
		_minor_color = get_color("grid_minor", "GraphEdit");
	} else {
		_minor_color = Color(1, 1, 1, 0.07);
	}

	if (has_color_override("line_major")) {
		_major_color = get_color("line_major");
	} else if (has_color("grid_major", "GraphEdit")) {
		// Reuse grid colors from GraphEdit.
		_major_color = get_color("grid_major", "GraphEdit");
	} else {
		_major_color = Color(1, 1, 1, 0.15);
	}

	if (origin_axes_visible) {
		if (has_color_override("axis_x")) {
			_axis_x_color = get_color("axis_x");
		} else if (has_color("axis_x_color", "Editor")) {
			_axis_x_color = get_color("axis_x_color", "Editor");
		} else {
			_axis_x_color = Color(0.96, 0.20, 0.32);
		}

		if (has_color_override("axis_y")) {
			_axis_y_color = get_color("axis_y");
		} else if (has_color("axis_y_color", "Editor")) {
			_axis_y_color = get_color("axis_y_color", "Editor");
		} else {
			_axis_y_color = Color(0.53, 0.84, 0.01);
		}
	}
}

void GridRect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_colors();
			update();
		} break;
		case NOTIFICATION_DRAW: {
			if (has_color_override("background")) {
				draw_rect(Rect2(Vector2(), get_size()), get_color("background"));
			} else if (has_stylebox("bg", "GraphEdit")) {
				// Reuse GraphEdit's styling.
				draw_style_box(get_stylebox("bg", "GraphEdit"), Rect2(Vector2(), get_size()));
			} else {
				draw_rect(Rect2(Vector2(), get_size()), Color::html("#2C2A32"));
			}
			const Vector2 &size = get_size() / origin_scale.abs();
			Vector2 ofs = origin_offset / origin_scale.abs();
			if (origin_centered) {
				ofs += -size / 2;
			}
			const Vector2 &from = (ofs / cell_size).floor();
			const Vector2 &to = (size / cell_size).floor() + Vector2(1, 1);

			// Draw minor lines first to prevent cross artifacts.
			_draw_grid_horizontal(from.y, from.y + to.y, ofs, LINE_MINOR);
			_draw_grid_vertical(from.x, from.x + to.x, ofs, LINE_MINOR);
			// Draw major lines on top of minor ones.
			_draw_grid_horizontal(from.y, from.y + to.y, ofs, LINE_MAJOR);
			_draw_grid_vertical(from.x, from.x + to.x, ofs, LINE_MAJOR);
			// Draw main axes last.
			_draw_grid_horizontal(from.y, from.y + to.y, ofs, LINE_AXIS);
			_draw_grid_vertical(from.x, from.x + to.x, ofs, LINE_AXIS);

			if (!Engine::get_singleton()->is_editor_hint() && metadata_show_tooltip) {
				String hint_tooltip = String(_point_snapped);
				Variant data = get_metadata(_point_snapped);
				if (data.get_type() != Variant::NIL) {
					String data_str;
					VariantWriter::write_to_string(data, data_str);
					hint_tooltip += "\n" + data_str;
				}
				set_tooltip(hint_tooltip);
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_colors();
		} break;
	}
}

void GridRect::_get_property_list(List<PropertyInfo> *p_list) const {
	// Reuse overriding mechanism.
	Vector<String> props;
	props.push_back("line_minor");
	props.push_back("line_major");
	props.push_back("axis_x");
	props.push_back("axis_y");
	props.push_back("background");

	for (int i = 0; i < props.size(); ++i) {
		const String &prop = props[i];
		uint32_t usage = PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_CHECKABLE;
		if (has_color_override(prop)) {
			usage |= PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_CHECKED;
		}
		PropertyInfo pi;
		pi.name = "custom_colors/" + prop;
		pi.type = Variant::COLOR;
		pi.hint = PROPERTY_HINT_NONE;
		pi.usage = usage;
		p_list->push_back(pi);
	}
}

void GridRect::_validate_property(PropertyInfo &property) const {
	if (property.name == "origin_axes_line_width") {
		if (!origin_axes_visible) {
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
}

void GridRect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &GridRect::set_cell_size);
	ClassDB::bind_method(D_METHOD("get_cell_size"), &GridRect::get_cell_size);

	ClassDB::bind_method(D_METHOD("set_cell_origin", "origin"), &GridRect::set_cell_origin);
	ClassDB::bind_method(D_METHOD("get_cell_origin"), &GridRect::get_cell_origin);

	ClassDB::bind_method(D_METHOD("set_cell_line_width", "width"), &GridRect::set_cell_line_width);
	ClassDB::bind_method(D_METHOD("get_cell_line_width"), &GridRect::get_cell_line_width);

	ClassDB::bind_method(D_METHOD("set_origin_offset", "offset"), &GridRect::set_origin_offset);
	ClassDB::bind_method(D_METHOD("get_origin_offset"), &GridRect::get_origin_offset);

	ClassDB::bind_method(D_METHOD("set_origin_scale", "scale"), &GridRect::set_origin_scale);
	ClassDB::bind_method(D_METHOD("get_origin_scale"), &GridRect::get_origin_scale);

	ClassDB::bind_method(D_METHOD("set_origin_centered", "centered"), &GridRect::set_origin_centered);
	ClassDB::bind_method(D_METHOD("is_origin_centered"), &GridRect::is_origin_centered);

	ClassDB::bind_method(D_METHOD("set_origin_axes_visible", "visible"), &GridRect::set_origin_axes_visible);
	ClassDB::bind_method(D_METHOD("is_origin_axes_visible"), &GridRect::is_origin_axes_visible);

	ClassDB::bind_method(D_METHOD("set_origin_axes_line_width", "width"), &GridRect::set_origin_axes_line_width);
	ClassDB::bind_method(D_METHOD("get_origin_axes_line_width"), &GridRect::get_origin_axes_line_width);

	ClassDB::bind_method(D_METHOD("set_subdivisions_horizontal", "count"), &GridRect::set_subdivisions_horizontal);
	ClassDB::bind_method(D_METHOD("get_subdivisions_horizontal"), &GridRect::get_subdivisions_horizontal);

	ClassDB::bind_method(D_METHOD("set_subdivisions_vertical", "count"), &GridRect::set_subdivisions_vertical);
	ClassDB::bind_method(D_METHOD("get_subdivisions_vertical"), &GridRect::get_subdivisions_vertical);

	ClassDB::bind_method(D_METHOD("set_subdivisions_line_width", "width"), &GridRect::set_subdivisions_line_width);
	ClassDB::bind_method(D_METHOD("get_subdivisions_line_width"), &GridRect::get_subdivisions_line_width);

	ClassDB::bind_method(D_METHOD("set_metadata_show_tooltip", "enabled"), &GridRect::set_metadata_show_tooltip);
	ClassDB::bind_method(D_METHOD("is_showing_metadata_tooltip"), &GridRect::is_showing_metadata_tooltip);

	ClassDB::bind_method(D_METHOD("view_to_point", "position"), &GridRect::view_to_point);
	ClassDB::bind_method(D_METHOD("view_to_point_snapped", "position"), &GridRect::view_to_point_snapped);
	ClassDB::bind_method(D_METHOD("point_to_view", "point"), &GridRect::point_to_view);

	ClassDB::bind_method(D_METHOD("_gui_input"), &GridRect::_gui_input);

	BIND_VMETHOD(MethodInfo(Variant::BOOL, "_draw_line",
			PropertyInfo(Variant::VECTOR2, "from"),
			PropertyInfo(Variant::VECTOR2, "to"),
			PropertyInfo(Variant::COLOR, "color"),
			PropertyInfo(Variant::REAL, "width"),
			PropertyInfo(Variant::DICTIONARY, "line")));

	BIND_ENUM_CONSTANT(CELL_ORIGIN_TOP_LEFT);
	BIND_ENUM_CONSTANT(CELL_ORIGIN_CENTER);

	BIND_ENUM_CONSTANT(AXIS_X);
	BIND_ENUM_CONSTANT(AXIS_Y);

	BIND_ENUM_CONSTANT(LINE_MINOR);
	BIND_ENUM_CONSTANT(LINE_MAJOR);
	BIND_ENUM_CONSTANT(LINE_AXIS);

	ADD_GROUP("Cell", "cell");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cell_size"), "set_cell_size", "get_cell_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cell_origin", PROPERTY_HINT_ENUM, "Top-left,Center"), "set_cell_origin", "get_cell_origin");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "cell_line_width", PROPERTY_HINT_RANGE, "0.0,5.0,0.5,or_greater"), "set_cell_line_width", "get_cell_line_width");

	ADD_GROUP("Origin", "origin");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "origin_offset"), "set_origin_offset", "get_origin_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "origin_scale"), "set_origin_scale", "get_origin_scale");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "origin_centered"), "set_origin_centered", "is_origin_centered");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "origin_axes_visible"), "set_origin_axes_visible", "is_origin_axes_visible");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "origin_axes_line_width", PROPERTY_HINT_RANGE, "0.0,5.0,0.5,or_greater"), "set_origin_axes_line_width", "get_origin_axes_line_width");

	ADD_GROUP("Subdivisions", "subdivisions");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "subdivisions_horizontal", PROPERTY_HINT_RANGE, "0,16,1,or_greater"), "set_subdivisions_horizontal", "get_subdivisions_horizontal");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "subdivisions_vertical", PROPERTY_HINT_RANGE, "0,16,1,or_greater"), "set_subdivisions_vertical", "get_subdivisions_vertical");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "subdivisions_line_width", PROPERTY_HINT_RANGE, "0.0,5.0,0.5,or_greater"), "set_subdivisions_line_width", "get_subdivisions_line_width");

	ADD_GROUP("Metadata", "metadata");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "metadata_show_tooltip"), "set_metadata_show_tooltip", "is_showing_metadata_tooltip");

	ADD_SIGNAL(MethodInfo("point_clicked", PropertyInfo(Variant::VECTOR2, "point"), PropertyInfo(Variant::VECTOR2, "point_snapped"), PropertyInfo(Variant::VECTOR2, "local_position")));
}

GridRect::GridRect() {
	set_clip_contents(true);

	// add_color_override("line_minor", get_color("grid_minor", "GraphEdit"));
	// add_color_override("line_major", get_color("grid_major", "GraphEdit"));
	// add_color_override("axis_x", Color(0.96, 0.20, 0.32));
	// add_color_override("axis_y", Color(0.53, 0.84, 0.01));
}
