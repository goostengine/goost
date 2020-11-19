#include "poly_node_2d_editor_plugin.h"
#include "editor/plugins/canvas_item_editor_plugin.h"

PolyNode2DEditor *PolyNode2DEditor::singleton = nullptr;

void PolyNode2DEditor::_set_node(Node *p_node) {
	node = Object::cast_to<PolyNode2D>(p_node);
	if (!node) {
		return;
	}
	if (node->get_class() != "PolyNode2D") {
		String reason = TTR("Can only edit points in PolyNode2D directly, convert outlines to single path first.");
		disable_polygon_editing(true, reason);
	} else {
		disable_polygon_editing(false, "");
	}
}

Node2D *PolyNode2DEditor::_get_node() const {
	return node;
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

void PolyNode2DEditor::_menu_option(int p_option) {
	AbstractPolygon2DEditor::_menu_option(p_option);
	if (!node) {
		return;
	}
	selected_menu_item = (Menu)p_option;
	switch (p_option) {
		case MENU_OPTION_CONVERT_OUTLINES_TO_SINGLE_PATH: {
			PolyNode2D *new_node = memnew(PolyNode2D);
			Array outlines = node->get_outlines();
			Vector<Point2> prev_points = node->get_points();
			if (!outlines.empty()) {
				const Vector<Point2> &points = outlines[0];
				new_node->set_points(points);
			}
			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action(TTR("Convert Outlines To Single Path"));
			ur->add_do_method(node, "set_points", Vector<Point2>());
			ur->add_do_method(EditorNode::get_singleton()->get_scene_tree_dock(), "replace_node", node, new_node, true, false);
			ur->add_do_reference(new_node);
			ur->add_undo_method(EditorNode::get_singleton()->get_scene_tree_dock(), "replace_node", new_node, node, false, false);
			ur->add_undo_reference(node);
			ur->add_undo_method(node, "set_points", prev_points);
			ur->commit_action();
		} break;
	}
}

PolyNode2DEditor::PolyNode2DEditor(EditorNode *p_editor) :
		AbstractPolygon2DEditor(p_editor) {
	if (!singleton) {
		singleton = this;
	}
	options = memnew(MenuButton);
	CanvasItemEditor::get_singleton()->add_control_to_menu_panel(options);
	options->hide();

	options->set_text(TTR("PolyNode2D"));
	options->set_icon(EditorNode::get_singleton()->get_gui_base()->get_icon("Polygon2D", "EditorIcons"));

	options->get_popup()->add_item(TTR("Convert Outlines To Single Path"), MENU_OPTION_CONVERT_OUTLINES_TO_SINGLE_PATH);
	options->set_switch_on_hover(true);

	options->get_popup()->connect("id_pressed", this, "_menu_option");
}

void PolyNode2DEditorPlugin::make_visible(bool p_visible) {
	AbstractPolygon2DEditorPlugin::make_visible(p_visible);
	if (p_visible) {
		PolyNode2DEditor::get_singleton()->options->show();
	} else {
		PolyNode2DEditor::get_singleton()->options->hide();
	}
}

PolyNode2DEditorPlugin::PolyNode2DEditorPlugin(EditorNode *p_node) :
		AbstractPolygon2DEditorPlugin(p_node, memnew(PolyNode2DEditor(p_node)), "PolyNode2D") {
}
