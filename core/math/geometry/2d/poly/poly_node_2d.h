#pragma once

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

protected:
	// Properties.
	Vector<Point2> points;
	Operation operation = OP_UNION;
	bool open = false; // Closed = polygon, Open = polyline.

	// Drawing.
	Ref<Texture> texture;
	Ref<Texture> normal_map;
	Size2 tex_scale = Size2(1, 1);
	Vector2 tex_ofs;
	float tex_rot = 0.0;

	Color color = Color(1, 1, 1, 1); // Texture is also modulated by this.
	bool filled = true; // For polygons, without texture.
	real_t line_width = 2.0; // For polylines and non-filled polygons, without texture.
	bool antialiased = false;

private:
	PolyNode2D *parent = nullptr;
	bool update_queued = false;

protected:
	void _notification(int p_what);
	virtual void _validate_property(PropertyInfo &property) const;
	static void _bind_methods();
	void _draw();

	Vector<Vector<Point2>> outlines;
	void _update_outlines();
	virtual Vector<Vector<Point2>> _build_outlines();
	void _queue_update();

public:
	void set_points(const Vector<Point2> &p_points);
	Vector<Point2> get_points() const { return points; }

	void set_operation(Operation p_operation);
	Operation get_operation() const { return operation; }

	void set_open(bool p_open);
	bool is_open() const { return open; }

	void set_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture() const { return texture; }

	void set_normal_map(const Ref<Texture> &p_texture);
	Ref<Texture> get_normal_map() const { return normal_map; }

	void set_texture_offset(const Vector2 &p_offset);
	Vector2 get_texture_offset() const { return tex_ofs; }

	void set_texture_rotation(float p_rot);
	float get_texture_rotation() const { return tex_rot; }

	void set_texture_rotation_degrees(float p_rot);
	float get_texture_rotation_degrees() const;

	void set_texture_scale(const Size2 &p_scale);
	Size2 get_texture_scale() const { return tex_scale; }

	void set_color(const Color &p_color);
	Color get_color() const { return color; }

	void set_filled(bool p_filled);
	bool is_filled() const { return filled; }

	void set_line_width(real_t p_line_width);
	real_t get_line_width() const { return line_width; }

	void set_antialiased(bool p_antialiased);
	bool is_antialiased() const { return antialiased; }

	PolyNode2D *new_child(const Vector<Point2> &p_points);
	bool is_inner() const;
	bool is_root() const { return !parent; }

	void make_from_outlines(const Array &p_outlines);
	Vector<Vector<Point2>> get_outlines(); // Queued.
	Vector<Vector<Point2>> build_outlines(); // Immediate.
	Array get_outlines_array();
	Array build_outlines_array();

	void clear();

#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;
#endif
	PolyNode2D();
};

VARIANT_ENUM_CAST(PolyNode2D::Operation);

template <>
struct VariantCaster<PolyNode2D *> {
	static _FORCE_INLINE_ PolyNode2D *cast(const Variant &p_variant) {
		return (PolyNode2D *)p_variant.operator Object *();
	}
};

