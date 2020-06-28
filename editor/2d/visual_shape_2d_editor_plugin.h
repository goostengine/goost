#ifndef GOOST_VISUAL_SHAPE_2D_EDITOR_PLUGIN_H
#define GOOST_VISUAL_SHAPE_2D_EDITOR_PLUGIN_H

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "scene/2d/visual_shape_2d.h"

class VisualShape2DEditor : public AbstractPolygon2DEditor {
	GDCLASS(VisualShape2DEditor, AbstractPolygon2DEditor);

	VisualShape2D *node;

	void _update_editing();

protected:
	virtual Node2D *_get_node() const;
	virtual void _set_node(Node *p_node);

	virtual void _set_polygon(int p_idx, const Variant &p_polygon) const;
	virtual Variant _get_polygon(int p_idx) const;

	virtual void _action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon);

	virtual bool _has_resource() const;
	virtual void _create_resource();

	static void _bind_methods();

public:
	VisualShape2DEditor(EditorNode *p_editor);
};

class VisualShape2DEditorPlugin : public AbstractPolygon2DEditorPlugin {
	GDCLASS(VisualShape2DEditorPlugin, AbstractPolygon2DEditorPlugin);

public:
	VisualShape2DEditorPlugin(EditorNode *p_node);
};

#endif // GOOST_VISUAL_SHAPE_2D_EDITOR_PLUGIN_H
