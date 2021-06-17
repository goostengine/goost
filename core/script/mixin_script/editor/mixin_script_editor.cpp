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
			ScriptEditor::get_singleton()->connect("editor_script_changed", this, "_on_editor_script_changed");
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

void MixinScriptEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_editor_script_changed"), &MixinScriptEditor::_on_editor_script_changed);
}

MixinScriptEditor::MixinScriptEditor() {
}

MixinScriptEditor::~MixinScriptEditor() {
}

void MixinScriptEditor::validate() {
}
