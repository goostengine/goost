#include "poly_node_2d_editor_plugin.h"
#include "editor/plugins/canvas_item_editor_plugin.h"

Node2D *PolyNode2DEditor::_get_node() const {
	return node;
}

void PolyNode2DEditor::_set_node(Node *p_line) {
	node = Object::cast_to<PolyNode2D>(p_line);
}

bool PolyNode2DEditor::_is_line() const {
	return node->is_open();
}

Variant PolyNode2DEditor::_get_polygon(int p_idx) const {
	return _get_node()->get("points");
}

void PolyNode2DEditor::_set_polygon(int p_idx, const Variant &p_polygon) const {
	_get_node()->set("points", p_polygon);
}

void PolyNode2DEditor::_action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon) {
	Node2D *node = _get_node();
	undo_redo->add_do_method(node, "set_points", p_polygon);
	undo_redo->add_undo_method(node, "set_points", p_previous);
}

PolyNode2DEditor::PolyNode2DEditor(EditorNode *p_editor) :
		AbstractPolygon2DEditor(p_editor) {
	node = nullptr;
}

PolyNode2DEditorPlugin::PolyNode2DEditorPlugin(EditorNode *p_node) :
		AbstractPolygon2DEditorPlugin(p_node, memnew(PolyNode2DEditor(p_node)), "PolyNode2D") {
}
