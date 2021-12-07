#pragma once

#include "editor/editor_plugin.h"

// General-purpose plugin to handle global events.

class GoostEditorPlugin : public EditorPlugin {
	GDCLASS(GoostEditorPlugin, EditorPlugin);
	
protected:
	static void _bind_methods();
	
	void _on_editor_scene_changed(Node *p_scene_root);

public:
	GoostEditorPlugin(EditorNode *p_editor);
};
