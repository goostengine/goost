#ifndef GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H
#define GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "goost/scene/2d/poly_shapes_2d.h"

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "scene/2d/line_2d.h"

class PolyNode2DEditor : public AbstractPolygon2DEditor {
	GDCLASS(PolyNode2DEditor, AbstractPolygon2DEditor);

	PolyNode2D *node;

protected:
	virtual Node2D *_get_node() const override;
	virtual void _set_node(Node *p_line) override;

	virtual bool _is_line() const override;
	virtual Variant _get_polygon(int p_idx) const override;
	virtual void _set_polygon(int p_idx, const Variant &p_polygon) const override;
	virtual void _action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon) override;

public:
	PolyNode2DEditor(EditorNode *p_editor);
};

class PolyNode2DEditorPlugin : public AbstractPolygon2DEditorPlugin {
	GDCLASS(PolyNode2DEditorPlugin, AbstractPolygon2DEditorPlugin);

public:
	PolyNode2DEditorPlugin(EditorNode *p_node);
};

#endif // GOOST_POLY_NODE_2D_EDITOR_PLUGIN_H
