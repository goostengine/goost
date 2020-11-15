#include "poly_node_2d.h"
#include "boolean/poly_boolean.h"
#include "decomp/poly_decomp.h"

void draw_polyline_closed(PolyNode2D *p_node, const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width = 1.0) {
	ERR_FAIL_COND(p_polyline.size() < 2);
	p_node->draw_polyline(p_polyline, p_color, p_width);
	p_node->draw_line(p_polyline[p_polyline.size() - 1], p_polyline[0], p_color, p_width);
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
			draw_polyline(outlines[i], color, width);
		}
	} else if (!filled) { // Non-filled polygons.
		for (int i = 0; i < outlines.size(); ++i) {
			draw_polyline_closed(this, outlines[i], color, width);
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
		const int indices_count = triangles.size() * 3;
		Vector<int> indices;
		for (int i = 0; i < indices_count; ++i) {
			indices.push_back(i);
		}
		Vector<Color> colors;
		colors.push_back(color);
		VS::get_singleton()->canvas_item_add_triangle_array(get_canvas_item(), indices, vertices, colors);
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
	if (property.name == "width") {
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

void PolyNode2D::set_color(const Color &p_color) {
	color = p_color;
	_queue_update();
}

void PolyNode2D::set_filled(bool p_filled) {
	filled = p_filled;
	_queue_update();
	_change_notify();
}

void PolyNode2D::set_width(real_t p_width) {
	width = p_width;
	_queue_update();
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

	ClassDB::bind_method(D_METHOD("set_color", "color"), &PolyNode2D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &PolyNode2D::get_color);

	ClassDB::bind_method(D_METHOD("set_filled", "filled"), &PolyNode2D::set_filled);
	ClassDB::bind_method(D_METHOD("is_filled"), &PolyNode2D::is_filled);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &PolyNode2D::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &PolyNode2D::get_width);

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
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "filled"), "set_filled", "is_filled");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "width"), "set_width", "get_width");
}

PolyNode2D::PolyNode2D() {
	set_notify_local_transform(true);
}
