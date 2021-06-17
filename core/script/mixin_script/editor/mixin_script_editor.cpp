#include "mixin_script_editor.h"

#include "core/script_language.h"
#include "editor/editor_node.h"

void MixinScriptEditor::apply_code() {
}

RES MixinScriptEditor::get_edited_resource() const {
	return script;
}

void MixinScriptEditor::set_edited_resource(const RES &p_res) {
	ERR_FAIL_COND(script.is_valid());
	ERR_FAIL_COND(p_res.is_null());
	script = p_res;
}

void MixinScriptEditor::enable_editor() {
}

Vector<String> MixinScriptEditor::get_functions() {
	return Vector<String>();
}

void MixinScriptEditor::reload_text() {
}

String MixinScriptEditor::get_name() {
	String name;

	if (script->get_path().find("local://") == -1 && script->get_path().find("::") == -1) {
		name = script->get_path().get_file();
		if (is_unsaved()) {
			name += "(*)";
		}
	} else if (script->get_name() != "") {
		name = script->get_name();
	} else {
		name = script->get_class() + "(" + itos(script->get_instance_id()) + ")";
	}

	return name;
}

Ref<Texture> MixinScriptEditor::get_icon() {
	return Control::get_icon("MixinScript", "EditorIcons");
}

bool MixinScriptEditor::is_unsaved() {
	return script->is_edited();
}

Variant MixinScriptEditor::get_edit_state() {
	Dictionary d;
	return d;
}

void MixinScriptEditor::set_edit_state(const Variant &p_state) {

}

void MixinScriptEditor::goto_line(int p_line, bool p_with_error) {

}

void MixinScriptEditor::set_executing_line(int p_line) {
}

void MixinScriptEditor::clear_executing_line() {
}

void MixinScriptEditor::trim_trailing_whitespace() {
}

void MixinScriptEditor::insert_final_newline() {
}

void MixinScriptEditor::convert_indent_to_spaces() {
}

void MixinScriptEditor::convert_indent_to_tabs() {
}

void MixinScriptEditor::ensure_focus() {
}

void MixinScriptEditor::tag_saved_version() {
}

void MixinScriptEditor::reload(bool p_soft) {
}

void MixinScriptEditor::get_breakpoints(List<int> *p_breakpoints) {

}

void MixinScriptEditor::add_callback(const String &p_function, PoolStringArray p_args) {

}

bool MixinScriptEditor::show_members_overview() {
	return false;
}

void MixinScriptEditor::update_settings() {

}

void MixinScriptEditor::set_debugger_active(bool p_active) {

}

void MixinScriptEditor::set_tooltip_request_func(String p_method, Object *p_obj) {
}

Control *MixinScriptEditor::get_edit_menu() {
	return nullptr;
}

void MixinScriptEditor::clear_edit_menu() {
}

void MixinScriptEditor::add_syntax_highlighter(SyntaxHighlighter *p_highlighter) {
}

void MixinScriptEditor::set_syntax_highlighter(SyntaxHighlighter *p_highlighter) {
}

void MixinScriptEditor::_notification(int p_what) {
	switch(p_what) {
		case NOTIFICATION_ENTER_TREE: {
			attach_main_script_button->set_icon(Control::get_icon("ScriptCreate", "EditorIcons"));

			ScriptEditor::get_singleton()->connect("editor_script_changed", this, "_on_editor_script_changed");
		} break;
		case NOTIFICATION_THEME_CHANGED:
		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {
			attach_main_script_button->set_icon(Control::get_icon("ScriptCreate", "EditorIcons"));
		} break;
	}
}

void MixinScriptEditor::_on_editor_script_changed(Ref<Script> p_script) {
	Ref<MixinScript> ms = p_script;
	if (ms.is_valid()) {
		Ref<Script> main_script = ms->get_main_script();
		if (main_script.is_valid()) {
			// Delegate editing to other script editors automatically.
			ScriptEditor::get_singleton()->edit(main_script);
		}
	}
}

void MixinScriptEditor::_on_attach_main_script_pressed() {
	ERR_FAIL_COND(script.is_null());
	Ref<MixinScript> ms = script;
	if (ms.is_valid()) {
		Ref<Script> main_script = ms->get_main_script();
		ERR_FAIL_COND_MSG(main_script.is_valid(), "Main script is already attached.");

		// Base name.
		String base_name = ms->get_instance_base_type();
		if (base_name.empty()) {
			Node *root = get_tree()->get_edited_scene_root();
			if (root) {
				base_name = root->get_class();
			}
		}
		if (base_name.empty()) {
			base_name = "Node";
		}
		// Base path.
		String base_path = ms->get_path().get_basename();
		if (base_path.empty()) {
			EditorNode::get_singleton()->show_warning(TTR("The MixinScript is not yet saved to disk.\nSave the script first to be able to assign the main script."));
			return;
		}
		ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
		sc->config(base_name, base_path);
		sc->popup_centered();
		sc->connect("script_created", this, "_on_main_script_created");
		sc->connect("popup_hide", this, "_on_main_script_creation_closed", varray(), CONNECT_ONESHOT);
	}
}

void MixinScriptEditor::_on_main_script_created(Ref<Script> p_script) {
	if (p_script.is_valid()) {
		Ref<MixinScript> ms = script;
		if (ms == p_script) {
			EditorNode::get_singleton()->show_warning(TTR("Cannot set a main script pointing to itself."));
			return;
		}
		ms->set_main_script(p_script);
		EditorNode::get_singleton()->save_resource(ms);
		EditorNode::get_singleton()->push_item(p_script.operator->());
	}
}

void MixinScriptEditor::_on_main_script_creation_closed() {
	ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
	sc->disconnect("script_created", this, "_on_main_script_created");
}

void MixinScriptEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_editor_script_changed"), &MixinScriptEditor::_on_editor_script_changed);
	ClassDB::bind_method(D_METHOD("_on_attach_main_script_pressed"), &MixinScriptEditor::_on_attach_main_script_pressed);
	ClassDB::bind_method(D_METHOD("_on_main_script_created"), &MixinScriptEditor::_on_main_script_created);
	ClassDB::bind_method(D_METHOD("_on_main_script_creation_closed"), &MixinScriptEditor::_on_main_script_creation_closed);
}

MixinScriptEditor::MixinScriptEditor() {
	container = memnew(CenterContainer);
	add_child(container);
	container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	container->set_anchors_and_margins_preset(Control::PRESET_WIDE);

	attach_main_script_button = memnew(Button);
	container->add_child(attach_main_script_button);
	attach_main_script_button->set_text(TTR("Attach Main Script"));
	attach_main_script_button->set_custom_minimum_size(Vector2(200, 50));
	attach_main_script_button->connect("pressed", this, "_on_attach_main_script_pressed");
}

MixinScriptEditor::~MixinScriptEditor() {
}

void MixinScriptEditor::validate() {
}
