#include "poly_node_2d.h"
#include "boolean/poly_boolean.h"
#include "decomp/poly_decomp.h"

#include "goost/core/math/2d/geometry/goost_geometry_2d.h"

void draw_polyline_open(PolyNode2D *p_node, const Vector<Point2> &p_polyline, const Color &p_color, real_t p_line_width = 1.0) {
	ERR_FAIL_COND(p_polyline.size() < 2);
	for (int i = 0; i < p_polyline.size() - 1; ++i) {
		p_node->draw_line(p_polyline[i], p_polyline[i + 1], p_color, p_line_width, p_node->is_antialiased());
	}
}

void draw_polyline_closed(PolyNode2D *p_node, const Vector<Point2> &p_polyline, const Color &p_color, real_t p_line_width = 1.0) {
	ERR_FAIL_COND(p_polyline.size() < 3);
	for (int i = 0; i < p_polyline.size(); ++i) {
		p_node->draw_line(p_polyline[i], p_polyline[(i + 1) % p_polyline.size()], p_color, p_line_width, p_node->is_antialiased());
	}
}

void PolyNode2D::_draw() {
	if (parent && operation != OP_NONE) {
		return;
	}
	if (outlines.empty()) {
		return;
	}
	if (open) { // Polylines.
		for (int i = 0; i < outlines.size(); ++i) {
			if (outlines[i].size() > 1) {
				draw_polyline_open(this, outlines[i], color, line_width);
			}
		}
	} else if (!filled) { // Non-filled polygons.
		for (int i = 0; i < outlines.size(); ++i) {
			if (outlines[i].size() >= 3) {
				draw_polyline_closed(this, outlines[i], color, line_width);
			}
		}
	} else { // Filled polygons.
		const Vector<Vector<Point2>> &triangles = PolyDecomp2D::triangulate_polygons(outlines);
		if (triangles.empty()) {
			return;
		}
		Vector<Point2> vertices;
		for (int i = 0; i < triangles.size(); ++i) {
			const Vector<Point2> &part = triangles[i];
			for (int j = 0; j < part.size(); ++j) {
				vertices.push_back(part[j]);
			}
		}
		Vector<Point2> uvs;
		if (texture.is_valid()) {
			Transform2D trans(tex_rot, tex_ofs);
			trans.scale(tex_scale);
			Size2 tex_size = texture->get_size();
			uvs.resize(vertices.size());
			for (int i = 0; i < vertices.size(); ++i) {
				uvs.write[i] = trans.xform(vertices[i]) / tex_size;
			}
		}
		const int indices_count = triangles.size() * 3;
		Vector<int> indices;
		for (int i = 0; i < indices_count; ++i) {
			indices.push_back(i);
		}
		Vector<Color> colors;
		colors.push_back(color);

		RID texture_rid = texture.is_valid() ? texture->get_rid() : RID();
		RID normal_map_rid = normal_map.is_valid() ? normal_map->get_rid() : RID();

		VS::get_singleton()->canvas_item_add_triangle_array(
				get_canvas_item(), indices, vertices, colors, uvs,
				Vector<int>(), Vector<float>(), texture_rid, -1, normal_map_rid, antialiased);
	}
}

void PolyNode2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_PARENTED: {
			parent = Object::cast_to<PolyNode2D>(get_parent());
			_queue_update();
		} break;
		case NOTIFICATION_DRAW: {
			if (!is_inside_tree()) {
				break;
			}
			_draw();
		} break;
		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED:
		case NOTIFICATION_VISIBILITY_CHANGED:
		case NOTIFICATION_EXIT_TREE: {
			_queue_update();
		} break;
	}
}

void PolyNode2D::_queue_update() {
	if (!is_inside_tree()) {
		return;
	}
	if (parent) {
		parent->_queue_update();
	} else if (!update_queued) {
		call_deferred("_update_outlines");
	}
	update_queued = true;
	update();
}

Vector<Vector<Point2>> copy_outlines(const Vector<Vector<Point2>> &p_outlines, const Transform2D &p_trans) {
	Vector<Vector<Point2>> outlines;

	for (int i = 0; i < p_outlines.size(); ++i) {
		Vector<Point2> poly = p_outlines[i];
		{
			Point2 *ptr = poly.ptrw();
			for (int j = 0; j < poly.size(); ++j) {
				ptr[j] = p_trans.xform(ptr[j]);
			}
		}
		outlines.push_back(poly);
	}
	return outlines;
}

Vector<Vector<Point2>> PolyNode2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	Vector<Point2> outer_points = points;
	if (Geometry::is_polygon_clockwise(outer_points)) {
		outer_points.invert();
	}
	outlines.push_back(points);

	return outlines;
}

Vector<Vector<Point2>> PolyNode2D::_get_outlines() {
	if (!update_queued) {
		return outlines;
	}
	outlines = _build_outlines();

	for (int i = 0; i < get_child_count(); ++i) {
		PolyNode2D *clip = Object::cast_to<PolyNode2D>(get_child(i));
		if (!clip) {
			continue;
		}
		if (!clip->is_visible_in_tree()) {
			continue;
		}
		Vector<Vector<Point2>> clip_outlines = clip->_get_outlines();
		if (clip_outlines.empty()) {
			continue;
		}
		if (outlines.empty()) {
			outlines = copy_outlines(clip_outlines, clip->get_transform());
		} else if (clip->operation != OP_NONE) {
			clip_outlines = copy_outlines(clip_outlines, clip->get_transform());
			auto op = PolyBoolean2D::Operation(clip->operation);

			if (open && !clip->open) { // Polylines vs Polygons.
				switch (op) {
					case OP_DIFFERENCE: {
						outlines = PolyBoolean2D::clip_polylines_with_polygons(outlines, clip_outlines);
					} break;
					case OP_INTERSECTION: {
						outlines = PolyBoolean2D::intersect_polylines_with_polygons(outlines, clip_outlines);
					} break;
				}
			} else { // Polygons vs Polygons.
				outlines = PolyBoolean2D::boolean_polygons(outlines, clip_outlines, op);
			}
		}
	}
	update_queued = false;

	return outlines;
}

void PolyNode2D::_update_outlines() {
	if (parent) {
		return;
	}
	_get_outlines();
	update();
}

void PolyNode2D::_validate_property(PropertyInfo &property) const {
	if (property.name == "filled") {
		if (open) {
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
	if (property.name == "line_width") {
		if (!open && filled) {
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
}

void PolyNode2D::set_points(const Vector<Point2> &p_points) {
	points = p_points;
	_queue_update();
}

void PolyNode2D::set_operation(Operation p_operation) {
	operation = p_operation;
	_queue_update();
}

void PolyNode2D::set_open(bool p_open) {
	open = p_open;
	_queue_update();
	_change_notify();
}

void PolyNode2D::set_texture(const Ref<Texture> &p_texture) {
	texture = p_texture;
	update();
}

void PolyNode2D::set_normal_map(const Ref<Texture> &p_texture) {
	normal_map = p_texture;
	update();
}

void PolyNode2D::set_texture_offset(const Vector2 &p_offset) {
	tex_ofs = p_offset;
	update();
}

void PolyNode2D::set_texture_rotation(float p_rot) {
	tex_rot = p_rot;
	update();
}

void PolyNode2D::set_texture_rotation_degrees(float p_rot) {
	set_texture_rotation(Math::deg2rad(p_rot));
	update();
}

float PolyNode2D::get_texture_rotation_degrees() const {
	return Math::rad2deg(get_texture_rotation());
}

void PolyNode2D::set_texture_scale(const Size2 &p_scale) {
	tex_scale = p_scale;
	update();
}

void PolyNode2D::set_color(const Color &p_color) {
	color = p_color;
	update();
}

void PolyNode2D::set_filled(bool p_filled) {
	filled = p_filled;
	update();
	_change_notify();
}

void PolyNode2D::set_line_width(real_t p_line_width) {
	line_width = p_line_width;
	update();
}

void PolyNode2D::set_antialiased(bool p_antialiased) {
	antialiased = p_antialiased;
	update();
}

PolyNode2D *PolyNode2D::new_child(const Vector<Point2> &p_points) {
	PolyNode2D *child = memnew(PolyNode2D);
	child->points = p_points;
	add_child(child);
	return child;
}

bool PolyNode2D::is_inner() const {
	PolyNode2D *n = Object::cast_to<PolyNode2D>(get_parent());
	bool inner = !n && points.empty();
	while (n) {
		inner = !inner;
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
	return inner;
}

void PolyNode2D::make_from_outlines(const Array &p_outlines) {
	ERR_FAIL_COND(p_outlines.empty());

	clear();
	Vector<Vector<Point2>> outlines;
	for (int i = 0; i < p_outlines.size(); i++) {
		outlines.push_back(p_outlines[i]);
	}
	PolyNode2D *tree = memnew(PolyNode2D);
	PolyBoolean2D::boolean_polygons_tree(
			outlines, Vector<Vector<Point2>>(), PolyBoolean2D::OP_NONE, tree);

	if (tree->get_child_count() > 0) {
		PolyNode2D *outer = Object::cast_to<PolyNode2D>(tree->get_child(0));
		ERR_FAIL_COND(!outer);
		points = outer->points;
		for (int i = 0; i < outer->get_child_count(); ++i) {
			PolyNode2D *inner = Object::cast_to<PolyNode2D>(outer->get_child(i));
			outer->remove_child(inner);
			if (Geometry::is_polygon_clockwise(inner->points)) {
				inner->operation = OP_DIFFERENCE; // Cut a hole.
			}
			add_child(inner);
		}
	}
	memdelete(tree);
	update();
}

Array PolyNode2D::get_outlines() {
	Array ret;
	for (int i = 0; i < outlines.size(); ++i) {
		ret.push_back(outlines[i]);
	}
	return ret;
}

void PolyNode2D::clear() {
	while (get_child_count() > 0) {
		const int idx = get_child_count() - 1;
		PolyNode2D *child = Object::cast_to<PolyNode2D>(get_child(idx));
		if (child) {
			remove_child(child);
			memdelete(child);
		}
	}
	points = Vector<Point2>();
}

void PolyNode2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update_outlines"), &PolyNode2D::_update_outlines);

	ClassDB::bind_method(D_METHOD("set_points", "points"), &PolyNode2D::set_points);
	ClassDB::bind_method(D_METHOD("get_points"), &PolyNode2D::get_points);

	ClassDB::bind_method(D_METHOD("set_operation", "operation"), &PolyNode2D::set_operation);
	ClassDB::bind_method(D_METHOD("get_operation"), &PolyNode2D::get_operation);

	ClassDB::bind_method(D_METHOD("set_open", "open"), &PolyNode2D::set_open);
	ClassDB::bind_method(D_METHOD("is_open"), &PolyNode2D::is_open);

	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &PolyNode2D::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &PolyNode2D::get_texture);

	ClassDB::bind_method(D_METHOD("set_normal_map", "normal_map"), &PolyNode2D::set_normal_map);
	ClassDB::bind_method(D_METHOD("get_normal_map"), &PolyNode2D::get_normal_map);

	ClassDB::bind_method(D_METHOD("set_texture_offset", "texture_offset"), &PolyNode2D::set_texture_offset);
	ClassDB::bind_method(D_METHOD("get_texture_offset"), &PolyNode2D::get_texture_offset);

	ClassDB::bind_method(D_METHOD("set_texture_rotation", "texture_rotation"), &PolyNode2D::set_texture_rotation);
	ClassDB::bind_method(D_METHOD("get_texture_rotation"), &PolyNode2D::get_texture_rotation);

	ClassDB::bind_method(D_METHOD("set_texture_rotation_degrees", "texture_rotation"), &PolyNode2D::set_texture_rotation_degrees);
	ClassDB::bind_method(D_METHOD("get_texture_rotation_degrees"), &PolyNode2D::get_texture_rotation_degrees);

	ClassDB::bind_method(D_METHOD("set_texture_scale", "texture_scale"), &PolyNode2D::set_texture_scale);
	ClassDB::bind_method(D_METHOD("get_texture_scale"), &PolyNode2D::get_texture_scale);

	ClassDB::bind_method(D_METHOD("set_color", "color"), &PolyNode2D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &PolyNode2D::get_color);

	ClassDB::bind_method(D_METHOD("set_filled", "filled"), &PolyNode2D::set_filled);
	ClassDB::bind_method(D_METHOD("is_filled"), &PolyNode2D::is_filled);

	ClassDB::bind_method(D_METHOD("set_line_width", "line_width"), &PolyNode2D::set_line_width);
	ClassDB::bind_method(D_METHOD("get_line_width"), &PolyNode2D::get_line_width);

	ClassDB::bind_method(D_METHOD("set_antialiased", "antialiased"), &PolyNode2D::set_antialiased);
	ClassDB::bind_method(D_METHOD("is_antialiased"), &PolyNode2D::is_antialiased);

	ClassDB::bind_method(D_METHOD("new_child", "from_points"), &PolyNode2D::new_child);

	ClassDB::bind_method(D_METHOD("is_inner"), &PolyNode2D::is_inner);
	ClassDB::bind_method(D_METHOD("is_root"), &PolyNode2D::is_root);

	ClassDB::bind_method(D_METHOD("make_from_outlines", "outlines"), &PolyNode2D::make_from_outlines);
	ClassDB::bind_method(D_METHOD("get_outlines"), &PolyNode2D::get_outlines);

	ClassDB::bind_method(D_METHOD("clear"), &PolyNode2D::clear);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "points"), "set_points", "get_points");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "operation", PROPERTY_HINT_ENUM, "None,Union,Difference,Intersection,Xor"), "set_operation", "get_operation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "open"), "set_open", "is_open");

	ADD_GROUP("Draw", "");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal_map", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_normal_map", "get_normal_map");
	ADD_GROUP("Draw", "texture_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "texture_offset"), "set_texture_offset", "get_texture_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "texture_scale"), "set_texture_scale", "get_texture_scale");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "texture_rotation_degrees", PROPERTY_HINT_RANGE, "-360,360,0.1,or_lesser,or_greater"), "set_texture_rotation_degrees", "get_texture_rotation_degrees");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "texture_rotation", PROPERTY_HINT_NONE, "", 0), "set_texture_rotation", "get_texture_rotation");

	ADD_GROUP("Draw", "");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "filled"), "set_filled", "is_filled");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "line_width", PROPERTY_HINT_RANGE, "1.0,5.0,0.1"), "set_line_width", "get_line_width");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "antialiased"), "set_antialiased", "is_antialiased");
}

#ifdef TOOLS_ENABLED
Rect2 PolyNode2D::_edit_get_rect() const {
	Rect2 rect = GoostGeometry2D::bounding_rect(points);
	if (rect == Rect2()) {
		if (!outlines.empty()) {
			for (int i = 0; i < outlines.size(); ++i) {
				const Rect2 &outline_rect = GoostGeometry2D::bounding_rect(outlines[i]);
				rect = rect.merge(outline_rect);
			}
		} else {
			rect = Rect2(-10, -10, 20, 20);
		}
	} else {
		rect.position -= rect.size * 0.3;
		rect.size += rect.size * 0.6;
	}
	return rect;
}

bool PolyNode2D::_edit_use_rect() const {
	return true;
}

bool PolyNode2D::_edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const {
	if (open) {
		if (points.size() >= 2) {
			const Point2 *ptr = points.ptr();
			for (int i = 0; i < points.size() - 1; ++i) {
				Vector2 p = Geometry::get_closest_point_to_segment_2d(p_point, &ptr[i]);
				if (p.distance_to(p_point) <= p_tolerance) {
					return true;
				}
			}
		}
	} else {
		if (!outlines.empty()) {
			bool inside = false;
			for (int i = 0; i < outlines.size(); ++i) {
				const Vector<Point2> &outline = outlines[i];
				if (outline.size() < 3) {
					continue;
				}
				inside = inside || static_cast<bool>(GoostGeometry2D::point_in_polygon(p_point, outline));
				if (inside) {
					return true;
				}
			}
			return false;
		}
	}
	return false;
}
#endif

PolyNode2D::PolyNode2D() {
	set_notify_local_transform(true);
}
