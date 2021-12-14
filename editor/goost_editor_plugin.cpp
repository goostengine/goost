#include "goost_editor_plugin.h"

#include "scene/2d/debug_2d.h"

#include "goost/classes_enabled.gen.h"

void GoostEditorPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_editor_scene_changed"), &GoostEditorPlugin::_on_editor_scene_changed);
}

void GoostEditorPlugin::_on_editor_scene_changed(Node *p_scene_root) {
#if defined(GOOST_SCENE_ENABLED) && defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_Debug2D)
	if (Debug2D::get_singleton()) {
		Debug2D::get_singleton()->clear();
	}
#endif
}

GoostEditorPlugin::GoostEditorPlugin(EditorNode *p_editor) {
	// Signals are added during `EditorPlugin::_bind_methods()`, so calling deferred.
	call_deferred("connect", "scene_changed", this, "_on_editor_scene_changed");
}
