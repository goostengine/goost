#include "visual_shape_2d_editor_plugin.h"

#include "editor/plugins/canvas_item_editor_plugin.h"

#include "scene/resources/concave_polygon_shape_2d.h"
#include "scene/resources/convex_polygon_shape_2d.h"

Node2D *VisualShape2DEditor::_get_node() const {
	return node;
}

void VisualShape2DEditor::_set_node(Node *p_node) {
	if (node) {
		node->disconnect("shape_changed", this, "_update_editing");
	}
	node = Object::cast_to<VisualShape2D>(p_node);
	if (node) {
		node->connect("shape_changed", this, "_update_editing");
	}
	_update_editing();
}

void VisualShape2DEditor::_set_polygon(int p_idx, const Variant &p_polygon) const {
	VisualShape2D *n = Object::cast_to<VisualShape2D>(_get_node());

	const Ref<Shape2D> &shape = n->get_shape();
	Ref<ConvexPolygonShape2D> convex = shape;
	Ref<ConcavePolygonShape2D> concave = shape;

	if (convex.is_valid()) {
		convex->set("points", p_polygon);
	} else if (concave.is_valid()) {
		concave->set("segments", p_polygon);
	}
}

Variant VisualShape2DEditor::_get_polygon(int p_idx) const {
	VisualShape2D *n = Object::cast_to<VisualShape2D>(_get_node());

	const Ref<Shape2D> &shape = n->get_shape();
	Ref<ConvexPolygonShape2D> convex = shape;
	Ref<ConcavePolygonShape2D> concave = shape;

	if (convex.is_valid()) {
		return convex->get("points");
	} else if (concave.is_valid()) {
		return concave->get("segments");
	}
	return Variant();
}

void VisualShape2DEditor::_update_editing() {
	if (!node) {
		return;
	}
	Ref<Shape2D> shape = node->get_shape();
	Ref<ConvexPolygonShape2D> convex = shape;
	Ref<ConcavePolygonShape2D> concave = shape;

	const String reason = TTR("No polygon-based shape resource is set.");

	if (shape.is_null()) {
		disable_polygon_editing(true, reason);
	} else if (convex.is_valid() || concave.is_valid()) {
		disable_polygon_editing(false, "");
	} else {
		disable_polygon_editing(true, reason);
	}
}

void VisualShape2DEditor::_action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon) {
	undo_redo->add_do_method(this, "_set_polygon", 0, p_polygon);
	undo_redo->add_undo_method(this, "_set_polygon", 0, p_previous);
}

bool VisualShape2DEditor::_has_resource() const {
	if (!node) {
		return false;
	}
	return node->get_shape().is_valid();
}

void VisualShape2DEditor::_create_resource() {
	if (!node) {
		return;
	}
	undo_redo->create_action(TTR("Create Convex Polygon Shape"));
	undo_redo->add_do_method(node, "set_shape", Ref<ConvexPolygonShape2D>(memnew(ConvexPolygonShape2D)));
	undo_redo->add_undo_method(node, "set_shape", Variant(REF()));
	undo_redo->commit_action();

	_menu_option(MODE_CREATE);

	EditorNode::get_singleton()->get_inspector()->refresh();
}

void VisualShape2DEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_set_polygon"), &VisualShape2DEditor::_set_polygon);
	ClassDB::bind_method(D_METHOD("_update_editing"), &VisualShape2DEditor::_update_editing);
}

VisualShape2DEditor::VisualShape2DEditor(EditorNode *p_editor) :
		AbstractPolygon2DEditor(p_editor) {
	node = nullptr;
}

VisualShape2DEditorPlugin::VisualShape2DEditorPlugin(EditorNode *p_node) :
		AbstractPolygon2DEditorPlugin(p_node, memnew(VisualShape2DEditor(p_node)), "VisualShape2D") {
}
