#include "poly_shape_2d.h"

#include "core/engine.h"
#include "core/math/geometry.h"

#include "goost/core/math/2d/geometry/goost_geometry_2d.h"
#include "goost/core/math/2d/geometry/poly/decomp/poly_decomp.h"

Vector<Vector<Point2>> PolyShape2D::_collect_outlines() {
	Vector<Vector<Point2>> ret;
	for (int i = 0; i < get_child_count(); ++i) {
		PolyNode2D *n = Object::cast_to<PolyNode2D>(get_child(i));
		if (!n) {
			continue;
		}
		if (!n->is_visible_in_tree()) {
			continue;
		}
		Transform2D trans = n->get_transform();
		const Vector<Vector<Point2>> &outlines = n->build_outlines();
		for (int j = 0; j < outlines.size(); ++j) {
			Vector<Point2> poly = outlines[j];
			{
				Point2 *ptr = poly.ptrw();
				for (int k = 0; k < poly.size(); ++k) {
					ptr[k] = trans.xform(ptr[k]);
				}
			}
			ret.push_back(poly);
		}
	}
	return ret;
}

Vector<Vector<Point2>> PolyShape2D::_build_shapes() {
	shapes.clear();
	const Vector<Vector<Point2>> &outlines = _collect_outlines();
	if (outlines.empty()) {
		return shapes;
	}
	switch (build_mode) {
		case BUILD_TRIANGLES: {
			shapes.append_array(PolyDecomp2D::triangulate_polygons(outlines));
		} break;
		case BUILD_CONVEX: {
			shapes.append_array(PolyDecomp2D::decompose_polygons_into_convex(outlines));
		} break;
		case BUILD_SEGMENTS: {
			// Concave shapes cannot have inner outlines, so filter those out.
			for (int i = 0; i < outlines.size(); ++i) {
				const Vector<Point2> &points = outlines[i];
				if (!Geometry::is_polygon_clockwise(points)) {
					shapes.push_back(points);
				}
			}
		} break;
	}
	return shapes;
}

Array PolyShape2D::get_shapes_array() {
	Array ret;
	for (int i = 0; i < shapes.size(); ++i) {
		ret.push_back(shapes[i]);
	}
	return ret;
}

void PolyShape2D::add_child_notify(Node *p_child) {
	Node2D::add_child_notify(p_child);

	PolyNode2D *n = Object::cast_to<PolyNode2D>(p_child);
	if (!n) {
		return;
	}
	n->connect("outlines_updated", this, "_queue_update");
	_queue_update();
}

void PolyShape2D::remove_child_notify(Node *p_child) {
	Node2D::remove_child_notify(p_child);

	PolyNode2D *n = Object::cast_to<PolyNode2D>(p_child);
	if (!n) {
		return;
	}
	n->disconnect("outlines_updated", this, "_queue_update");
	_queue_update();
}

void PolyShape2D::_queue_update() {
	if (!is_inside_tree()) {
		return;
	}
	if (!update_queued) {
		call_deferred("_update_shapes");
	}
	update_queued = true;
}

void PolyShape2D::_update_shapes() {
	_build_shapes();
	auto script = get_script_instance();
	if (script && script->has_method("_apply_shapes")) {
		script->call("_apply_shapes");
		emit_signal("shapes_applied");
	} else {
		_apply_shapes();
		emit_signal("shapes_applied");
	}
	update();
	update_queued = false;
}

void PolyShape2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_build_shapes();
		} break;
		case NOTIFICATION_DRAW: {
			if (!Engine::get_singleton()->is_editor_hint() && !get_tree()->is_debugging_collisions_hint()) {
				break;
			}
			if (shapes.empty()) {
				break;
			}
			// Colors synced with `CollisionPolygon2D` node.
			Color c(0.4, 0.9, 0.1);
			for (int i = 0; i < shapes.size(); ++i) {
				const Vector<Point2> &points = shapes[i];
				if (points.size() < 3) {
					continue;
				}
				c.set_hsv(Math::fmod(c.get_h() + 0.738, 1), c.get_s(), c.get_v(), 0.5);
				draw_colored_polygon(points, c);
			}
		} break;
	}
}

void PolyShape2D::set_build_mode(BuildMode p_mode) {
	ERR_FAIL_INDEX((int)p_mode, 3);
	build_mode = p_mode;
	_queue_update();
}

String PolyShape2D::get_configuration_warning() const {
	String warning = Node2D::get_configuration_warning();

	bool found = false;
	for (int i = 0; i < get_child_count(); ++i) {
		PolyNode2D *n = Object::cast_to<PolyNode2D>(get_child(i));
		if (n) {
			found = true;
			break;
		}
	}
	if (!found) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("PolyNode2D is required to build a shape. Add PolyNode2D as a child.");
	}
	return warning;
}

void PolyShape2D::_bind_methods() {
	BIND_VMETHOD(MethodInfo(Variant::NIL, "_apply_shapes"));
	ClassDB::bind_method(D_METHOD("_update_shapes"), &PolyShape2D::_update_shapes);
	ClassDB::bind_method(D_METHOD("_queue_update"), &PolyShape2D::_queue_update);

	ClassDB::bind_method(D_METHOD("get_shapes"), &PolyShape2D::get_shapes_array);

	ClassDB::bind_method(D_METHOD("set_build_mode", "build_mode"), &PolyShape2D::set_build_mode);
	ClassDB::bind_method(D_METHOD("get_build_mode"), &PolyShape2D::get_build_mode);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "build_mode", PROPERTY_HINT_ENUM, "Triangles,Convex,Segments"), "set_build_mode", "get_build_mode");

	ADD_SIGNAL(MethodInfo("shapes_applied"));

	BIND_ENUM_CONSTANT(BUILD_TRIANGLES);
	BIND_ENUM_CONSTANT(BUILD_CONVEX);
	BIND_ENUM_CONSTANT(BUILD_SEGMENTS);
}
