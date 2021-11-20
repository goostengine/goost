#include "mixin_script_editor_plugin.h"

#include "editor/plugins/script_editor_plugin.h"

bool EditorInspectorPluginMixinScript::can_handle(Object *p_object) {
	return Object::cast_to<MixinScript>(p_object) != nullptr;
}

void EditorInspectorPluginMixinScript::parse_begin(Object *p_object) {
	MixinScript *ms = Object::cast_to<MixinScript>(p_object);
	if (!ms) {
		return;
	}
	script = Ref<MixinScript>(ms);

	Button *edit = memnew(Button);
	edit->set_text(TTR("Edit"));
	edit->set_icon(EditorNode::get_singleton()->get_gui_base()->get_icon("Edit", "EditorIcons"));
	edit->connect("pressed", this, "_on_edit_pressed");

	add_custom_control(edit);
}

void EditorInspectorPluginMixinScript::_on_edit_pressed() {
	ERR_FAIL_COND(script.is_null());
	
	ScriptEditor::get_singleton()->set_meta("_edit_mixin", true);
	ScriptEditor::get_singleton()->edit(script);
}

void EditorInspectorPluginMixinScript::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_edit_pressed"), &EditorInspectorPluginMixinScript::_on_edit_pressed);
}

MixinScriptEditorPlugin::MixinScriptEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginMixinScript> inspector_plugin;
	inspector_plugin.instance();
	add_inspector_plugin(inspector_plugin);

	if (!EditorSettings::get_singleton()->has_setting("text_editor/files/open_first_script_on_editing_mixin_script")) {
		EditorSettings::get_singleton()->set_setting("text_editor/files/open_first_script_on_editing_mixin_script", false);
	}
}
