#include "poly_node_2d.h"

#include "core/engine.h"

#include "boolean/poly_boolean.h"
#include "decomp/poly_decomp.h"

void PolyNode2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED: {
			_queue_update();
		} break;
		case NOTIFICATION_DRAW: {
			if (!is_inside_tree()) {
				break;
			}
			if (!is_root()) {
				break; // Only the root node is responsible for drawing.
			}
			Vector<Vector<Point2>> polygons, polylines;
			_collect_outlines(&polygons, &polylines);

			if (!polygons.empty()) {
				Vector<Vector<Point2>> triangles = PolyDecomp2D::triangulate_polygons(polygons);
				if (triangles.empty()) {
					break;
				}
				Vector<Point2> vertices;
				for (int i = 0; i < triangles.size(); ++i) {
					const Vector<Point2> &tri = triangles[i];
					for (int j = 0; j < tri.size(); ++j) {
						vertices.push_back(tri[j]);
					}
				}
				const int indices_count = triangles.size() * 3;
				Vector<int> indices;
				for (int i = 0; i < indices_count; ++i) {
					indices.push_back(i);
				}
				VS::get_singleton()->canvas_item_add_triangle_array(get_canvas_item(), indices, vertices, Vector<Color>());
			}
			if (!polylines.empty()) {
				for (int i = 0; i < polylines.size(); ++i) {
					draw_polyline(polylines[i], Color(1, 1, 1));
				}
			}
		} break;
		case NOTIFICATION_PARENTED: {
#ifdef TOOLS_ENABLED
			PolyNode2D *n = this;
			while (n) {
				if (n->is_root()) {
					n->update_configuration_warning();
					break;
				}
				n = Object::cast_to<PolyNode2D>(n->get_parent());
			}
#endif
		} break;
	}
}

void PolyNode2D::_queue_update() {
	PolyNode2D *n = this;
	while (n) {
		if (n->is_root()) {
			n->update();
			break;
		}
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
}

void PolyNode2D::_validate_property(PropertyInfo &property) const {
	if (is_root()) {
		if (property.name == "points" || property.name == "open") {
			// The root node acts as a container which has no outer boundary,
			// but technically treated as a hole.
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
}

String PolyNode2D::get_configuration_warning() const {
	String warning = Node2D::get_configuration_warning();
	bool has_children = false;
	for (int i = 0; i < get_child_count(); ++i) {
		PolyNode2D *child = Object::cast_to<PolyNode2D>(get_child(i));
		if (child) {
			has_children = true;
			break;
		}
	}
	if (is_root() && !has_children) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("The root node acts as a container for other nodes. Add a new PolyNode2D as child to create outer boundary.");
	}
	return warning;
}

void PolyNode2D::_collect_outlines(Vector<Vector<Point2>> *r_closed, Vector<Vector<Point2>> *r_open) {
	ERR_FAIL_NULL(r_closed);

	List<PolyNode2D *> to_visit;
	to_visit.push_back(this);

	while (!to_visit.empty()) {
		PolyNode2D *n = Object::cast_to<PolyNode2D>(to_visit.back()->get());
		to_visit.pop_back();
		if (!n) {
			continue;
		}
		for (int i = 0; i < n->get_child_count(); ++i) {
			PolyNode2D *child = Object::cast_to<PolyNode2D>(n->get_child(i));
			if (!child) {
				continue;
			}
			const Vector<Point2> &points = child->get_points();
			const Transform2D &trans = child->get_transform();
			Vector<Point2> poly;
			poly.resize(points.size());
			{
				Point2 *ptr = poly.ptrw();
				for (int i = 0; i < points.size(); ++i) {
					ptr[i] = trans.xform(points[i]);
				}
			}
			if (child->open && r_open) {
				r_open->push_back(poly);
			} else {
				r_closed->push_back(poly);
			}
			to_visit.push_back(child);
		}
	}
}

void PolyNode2D::set_points(const Vector<Point2> &p_points) {
	points = p_points;
	_queue_update();
}

void PolyNode2D::set_open(bool p_open) {
	open = p_open;
	_queue_update();
}

PolyNode2D *PolyNode2D::new_child(const Vector<Point2> &p_points) {
	PolyNode2D *child = memnew(PolyNode2D);
	child->points = p_points;
	add_child(child);
	return child;
}

bool PolyNode2D::is_hole() const {
	bool hole = true;
	PolyNode2D *n = Object::cast_to<PolyNode2D>(get_parent());
	while (n) {
		hole = !hole;
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
	return hole;
}

void PolyNode2D::create_from_polygons(const Array &p_polygons) {
	clear();

	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	PolyNode2D *result = PolyBoolean2D::boolean_polygons_tree(
			polygons, Vector<Vector<Point2>>(), PolyBoolean2D::OP_NONE);

	while (result->get_child_count() > 0) {
		const int idx = result->get_child_count() - 1;
		PolyNode2D *child = Object::cast_to<PolyNode2D>(result->get_child(idx));
		result->remove_child(child);
		add_child(child);
	}
	update();
}

Array PolyNode2D::create_objects() {
	Array objects;

	List<PolyNode2D *> to_visit;
	to_visit.push_back(this);

	while (!to_visit.empty()) {
		PolyNode2D *n = Object::cast_to<PolyNode2D>(to_visit.back()->get());
		to_visit.pop_back();
		if (!n) {
			continue;
		}
		for (int i = 0; i < n->get_child_count(); ++i) {
			PolyNode2D *outer = Object::cast_to<PolyNode2D>(n->get_child(i));
			if (!outer || outer->is_hole()) {
				continue;
			}
			PolyNode2D *root = memnew(PolyNode2D);
			PolyNode2D *new_outer = memnew(PolyNode2D);
			new_outer->points = outer->points;
			new_outer->open = outer->open;
			root->add_child(new_outer);

			for (int j = 0; j < outer->get_child_count(); ++j) {
				PolyNode2D *inner = Object::cast_to<PolyNode2D>(outer->get_child(j));
				if (!inner) {
					continue;
				}
				PolyNode2D *new_inner = memnew(PolyNode2D);
				new_inner->points = inner->points;
				new_inner->open = inner->open;
				new_outer->add_child(new_inner);

				to_visit.push_back(inner);
			}
			objects.push_back(root);
		}
	}
	return objects;
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
}

void PolyNode2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_points", "points"), &PolyNode2D::set_points);
	ClassDB::bind_method(D_METHOD("get_points"), &PolyNode2D::get_points);

	ClassDB::bind_method(D_METHOD("set_open", "open"), &PolyNode2D::set_open);
	ClassDB::bind_method(D_METHOD("is_open"), &PolyNode2D::is_open);

	ClassDB::bind_method(D_METHOD("new_child", "from_points"), &PolyNode2D::new_child);

	ClassDB::bind_method(D_METHOD("is_hole"), &PolyNode2D::is_hole);
	ClassDB::bind_method(D_METHOD("is_root"), &PolyNode2D::is_root);

	ClassDB::bind_method(D_METHOD("create_from_polygons", "polygons"), &PolyNode2D::create_from_polygons);
	ClassDB::bind_method(D_METHOD("create_objects"), &PolyNode2D::create_objects);

	ClassDB::bind_method(D_METHOD("clear"), &PolyNode2D::clear);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "points"), "set_points", "get_points");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "open"), "set_open", "is_open");
}

PolyNode2D::PolyNode2D() {
	set_notify_local_transform(true);
}
