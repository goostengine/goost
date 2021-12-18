#pragma once

#include "core/color.h"
#include "scene/gui/control.h"

class GridRect : public Control {
	GDCLASS(GridRect, Control);

public:
	enum CellOrigin {
		CELL_ORIGIN_TOP_LEFT,
		CELL_ORIGIN_CENTER,
	};
	enum Axis {
		AXIS_X,
		AXIS_Y,
	};
	enum Line {
		LINE_CELL,
		LINE_DIVISION,
		LINE_AXIS,
	};

protected:
	Vector2 cell_size = Vector2(32, 32);
	CellOrigin cell_origin = CELL_ORIGIN_TOP_LEFT;
	float cell_line_width = 1.0;

	int divisions_horizontal = 8;
	int divisions_vertical = 8;
	float divisions_line_width = 1.0;

	Vector2 origin_offset;
	Vector2 origin_scale = Vector2(1, 1);
	bool origin_centered = false;
	bool origin_axes_visible = false;
	float origin_axes_line_width = 1.0;

	bool metadata_show_tooltip = true;

public:
	void set_cell_size(const Vector2 &p_size);
	Vector2 get_cell_size() const { return cell_size; }

	void set_cell_origin(CellOrigin p_origin);
	CellOrigin get_cell_origin() const { return cell_origin; }

	void set_cell_line_width(float p_width);
	float get_cell_line_width() const { return cell_line_width; }

	void set_divisions_horizontal(int p_count);
	int get_divisions_horizontal() const { return divisions_horizontal; }

	void set_divisions_vertical(int p_count);
	int get_divisions_vertical() const { return divisions_vertical; }

	void set_divisions_line_width(float p_width);
	float get_divisions_line_width() const { return divisions_line_width; }

	void set_origin_offset(Vector2 p_offset);
	Vector2 get_origin_offset() const { return origin_offset; }

	void set_origin_scale(Vector2 p_scale);
	Vector2 get_origin_scale() const { return origin_scale; }

	void set_origin_centered(bool p_centered);
	bool is_origin_centered() const { return origin_centered; }

	void set_origin_axes_visible(bool p_visible);
	bool is_origin_axes_visible() const { return origin_axes_visible; }

	void set_origin_axes_line_width(float p_width);
	float get_origin_axes_line_width() const { return origin_axes_line_width; }

	void set_metadata_show_tooltip(bool p_enable);
	bool is_showing_metadata_tooltip() const { return metadata_show_tooltip; }

	Vector2 view_to_point(const Vector2 &p_position) const;
	Vector2 view_to_point_snapped(const Vector2 &p_position) const;
	Vector2 point_to_view(const Vector2 &p_point) const;

	// Should use Vector2i in Godot 4.x.
	void set_cell_metadata(const Vector2 &p_cell, const Variant &p_metadata);
	Variant get_cell_metadata(const Vector2 &p_cell) const;
	void clear_cell_metadata() { metadata.clear(); }

	virtual String get_tooltip(const Point2 &p_pos) const;

	GridRect();

protected:
	void _notification(int p_what);
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	virtual void _validate_property(PropertyInfo &property) const;
	void _gui_input(const Ref<InputEvent> &p_event);

	virtual bool _draw_line(const Vector2 &p_from, const Vector2 &p_to, const Color &p_color, float p_width, const Dictionary &p_line);

	Vector2 _get_final_offset(CellOrigin p_cell_origin) const;
	void _draw_grid_vertical(int from, int to, const Vector2 &p_ofs, Line p_type);
	void _draw_grid_horizontal(int from, int to, const Vector2 &p_ofs, Line p_type);
	void _update_colors();

private:
	Map<Vector2i, Variant> metadata;
	Dictionary current_line;

	Vector2 _point_snapped;

	Color _cell_color;
	Color _division_color;
	Color _axis_x_color;
	Color _axis_y_color;
};

VARIANT_ENUM_CAST(GridRect::CellOrigin);
VARIANT_ENUM_CAST(GridRect::Axis);
VARIANT_ENUM_CAST(GridRect::Line);

