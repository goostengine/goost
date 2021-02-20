#ifndef GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H
#define GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "goost/core/math/2d/geometry/poly/poly_node_2d.h"

class PolyNode2DEditor : public AbstractPolygon2DEditor {
	GDCLASS(PolyNode2DEditor, AbstractPolygon2DEditor);

	friend class PolyNode2DEditorPlugin;

protected:
	PolyNode2D *node = nullptr;

	enum Menu {
		MENU_OPTION_FLATTEN_OUTLINES = 100,
	};
	MenuButton *options = nullptr;
	Menu selected_menu_item;
	virtual void _menu_option(int p_option);

	static PolyNode2DEditor* singleton;

	virtual void _set_node(Node *p_node);
	virtual Node2D *_get_node() const;

	virtual bool _is_line() const;
	virtual Variant _get_polygon(int p_idx) const;
	virtual void _set_polygon(int p_idx, const Variant &p_polygon) const;
	virtual void _action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon);

public:
	static PolyNode2DEditor* get_singleton() { return singleton; }

	PolyNode2DEditor(EditorNode *p_editor);
};

class PolyNode2DEditorPlugin : public AbstractPolygon2DEditorPlugin {
	GDCLASS(PolyNode2DEditorPlugin, AbstractPolygon2DEditorPlugin);

public:
	virtual void make_visible(bool p_visible);

	PolyNode2DEditorPlugin(EditorNode *p_node);
};

#endif // GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H
