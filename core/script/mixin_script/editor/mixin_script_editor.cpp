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
	if (script.is_valid()) {
		if (script->is_connected("changed", this, "queue_update")) {
			script->disconnect("changed", this, "queue_update");
		}
	}
	script = p_res;
	if (script.is_valid()) {
		script->connect("changed", this, "queue_update");
	}
	queue_update();
}

void MixinScriptEditor::enable_editor() {
	// queue_update();
}

Vector<String> MixinScriptEditor::get_functions() {
	return Vector<String>();
}

void MixinScriptEditor::reload_text() {
	// queue_update();
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
	if (p_what == NOTIFICATION_THEME_CHANGED || p_what == NOTIFICATION_ENTER_TREE || EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED) {
		attach_main_script_button->set_icon(Control::get_icon("ScriptCreate", "EditorIcons"));
		add_mixin_button->set_icon(Control::get_icon("ScriptCreate", "EditorIcons"));

		if (script.is_valid() && script->get_main_script().is_valid()) {
			attach_main_script_button->set_icon(Control::get_icon("ScriptRemove", "EditorIcons"));
		}
		panel_main_script->add_style_override("panel", get_stylebox("bg", "Tree"));
		panel_mixins->add_style_override("panel", get_stylebox("bg", "Tree"));
	}
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			ScriptEditor::get_singleton()->connect("editor_script_changed", this, "_on_editor_script_changed");
		} break;
	}
}

void MixinScriptEditor::_on_editor_script_changed(Ref<Script> p_script) {
	// Edit `MixinScript` itself if requested explicitly via editor inspector.
	if (ScriptEditor::get_singleton()->has_meta("_edit_mixin")) {
		bool edit = (bool)ScriptEditor::get_singleton()->get_meta("_edit_mixin");
		if (edit) {
			// TODO: restore to delegating main script when you press edit
			// button in the MixinScript editor itself.
			ScriptEditor::get_singleton()->set_meta("_edit_mixin", false);
			return;
		}
	}
	// Otherwise, delegate editing to other script editors automatically.
	Ref<MixinScript> ms = p_script;
	if (ms.is_valid()) {
		Ref<Script> main_script = ms->get_main_script();
		if (main_script.is_valid()) {
			ScriptEditor::get_singleton()->edit(main_script);
		}
	}
}

void MixinScriptEditor::_on_attach_main_script_pressed() {
	ERR_FAIL_COND(script.is_null());

	if (script.is_valid()) {
		Ref<Script> main_script = script->get_main_script();
		if (main_script.is_valid()) {
			// Main script is already attached, detach now.
			script->set_main_script(Ref<Script>());
			EditorNode::get_singleton()->save_resource(script);
			queue_update();
			return;
		}
		// Base name.
		String base_name = script->get_instance_base_type();
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
		String base_path = script->get_path().get_basename();
		if (base_path.empty()) {
			EditorNode::get_singleton()->show_warning(TTR("The MixinScript is not yet saved to disk.\nPlease save the MixinScript first."));
			return;
		}
		ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
		sc->config(base_name, base_path);
		sc->popup_centered();
		sc->connect("script_created", this, "_on_main_script_created");
		sc->connect("popup_hide", this, "_on_main_script_creation_closed", varray(), CONNECT_ONESHOT);
	}
}

void MixinScriptEditor::_on_add_mixin_pressed() {
	ERR_FAIL_COND(script.is_null());

	String base_name = "Mixin";
	String base_path = script->get_path().get_basename();
	if (base_path.empty()) {
		EditorNode::get_singleton()->show_warning(TTR("The MixinScript is not yet saved to disk.\nPlease save the MixinScript first."));
		return;
	}
	ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
	sc->config(base_name, base_path);
	sc->popup_centered();
	sc->connect("script_created", this, "_on_mixin_created");
	sc->connect("popup_hide", this, "_on_mixin_creation_closed", varray(), CONNECT_ONESHOT);
}

void MixinScriptEditor::_on_main_script_created(Ref<Script> p_script) {
	if (p_script.is_valid()) {
		if (p_script == script) {
			EditorNode::get_singleton()->show_warning(TTR("Cannot set a main script pointing to itself."));
			return;
		}
		script->set_main_script(p_script);
		EditorNode::get_singleton()->save_resource(script);
		queue_update();
	}
}

void MixinScriptEditor::_on_main_script_creation_closed() {
	ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
	sc->disconnect("script_created", this, "_on_main_script_created");
}

void MixinScriptEditor::_on_main_script_button_pressed(Object *p_item, int p_column, int p_button) {
	TreeItem *ti = Object::cast_to<TreeItem>(p_item);
	switch (p_button) {
		case BUTTON_EDIT: {
			EditorNode::get_singleton()->push_item(script->get_main_script().ptr());
		} break;
		case BUTTON_REMOVE: {
			script->set_main_script(Ref<Script>());
		} break;
		default: {
		}
	}
	queue_update();
}

void MixinScriptEditor::_on_mixin_created(Ref<Script> p_script) {
	if (p_script.is_valid()) {
		for (int i = 0; i < script->get_mixin_count(); ++i) {
			Ref<Script> m = script->get_mixin(i);
			if (m == p_script) {
				EditorNode::get_singleton()->show_warning(TTR("MixinScript already has the script to be added, skipping."));
				return;
			}
		}
		script->add_mixin(p_script);
		EditorNode::get_singleton()->save_resource(script);
		queue_update();
	}
}

void MixinScriptEditor::_on_mixin_creation_closed() {
	ScriptCreateDialog *sc = EditorNode::get_singleton()->get_script_create_dialog();
	sc->disconnect("script_created", this, "_on_mixin_created");
}

static void shift_mixin(Ref<MixinScript> p_script, const Ref<Script> &p_mixin, int step) {
	int mixin_pos = -1;
	for (int i = 0; i < p_script->get_mixin_count(); ++i) {
		const Ref<Script> &s = p_script->get_mixin(i);
		if (s == p_mixin) {
			mixin_pos = i;
			break;
		}
	}
	ERR_FAIL_COND(mixin_pos == -1);
	p_script->move_mixin(CLAMP(mixin_pos + step, 0, p_script->get_mixin_count() - 1), p_mixin);
}

void MixinScriptEditor::_on_mixin_button_pressed(Object *p_item, int p_column, int p_button) {
	TreeItem *ti = Object::cast_to<TreeItem>(p_item);
	Ref<Script> mixin = p_item->get_meta("script");

	switch (p_button) {
		case BUTTON_EDIT: {
			EditorNode::get_singleton()->push_item(mixin.ptr());
		} break;
		case BUTTON_MOVE_UP: {
			shift_mixin(script, mixin, -1);
		} break;
		case BUTTON_MOVE_DOWN: {
			shift_mixin(script, mixin, +1);
		} break;
		case BUTTON_REMOVE: {
			for (int i = 0; i < script->get_mixin_count(); ++i) {
				Ref<Script> m = script->get_mixin(i);
				if (m == mixin) {
					script->remove_mixin(i);
					break;
				}
			}
		} break;
	}
	queue_update();
}

void MixinScriptEditor::queue_update() {
	if (update_queued) {
		return;
	}
	update_queued = true;
	call_deferred("_update_list");
}

void MixinScriptEditor::_update_list() {
	// Main script.
	tree_main_script->clear();
	Ref<Script> main_script = script->get_main_script();

	if (main_script.is_valid()) {
		TreeItem *root = tree_main_script->create_item();

		TreeItem *main_script_item = tree_main_script->create_item(root);
		String main_script_path = main_script->get_path();
		String main_script_name = main_script_path.get_file();

		main_script_item->set_icon(0, Control::get_icon(main_script->get_language()->get_type(), "EditorIcons"));
		main_script_item->set_text(0, main_script_name);
		main_script_item->set_text(1, main_script_path);

		main_script_item->add_button(2, Control::get_icon("Edit", "EditorIcons"), BUTTON_EDIT);
		main_script_item->add_button(2, Control::get_icon("ScriptRemove", "EditorIcons"), BUTTON_REMOVE);

		tree_main_script->show();
		panel_main_script->hide();
		attach_main_script_button->set_icon(Control::get_icon("ScriptRemove", "EditorIcons"));
		attach_main_script_button->set_text(TTR("Detach Main Script"));
		attach_main_script_button->set_tooltip(TTR("Detach main script from the object."));
	} else {
		tree_main_script->hide();
		panel_main_script->show();
		attach_main_script_button->set_icon(Control::get_icon("ScriptCreate", "EditorIcons"));
		attach_main_script_button->set_text(TTR("Attach Main Script"));
		attach_main_script_button->set_tooltip(TTR("Attach main script to edit this script like a regular script."));
	}
	// Mixins.
	tree_mixins->clear();
	TreeItem *mixin_root = tree_mixins->create_item();

	for (int i = 0; i < script->get_mixin_count(); ++i) {
		Ref<Script> mixin = script->get_mixin(i);

		TreeItem *mixin_item = tree_mixins->create_item(mixin_root);
		String mixin_path = mixin->get_path();
		String mixin_name = mixin_path.get_file();

		mixin_item->set_text(0, itos(i + 1));

		mixin_item->set_icon(1, Control::get_icon(mixin->get_language()->get_type(), "EditorIcons"));
		mixin_item->set_text(1, mixin_name);

		mixin_item->set_text(2, mixin_path);

		mixin_item->add_button(3, Control::get_icon("Edit", "EditorIcons"), BUTTON_EDIT);
		mixin_item->add_button(3, Control::get_icon("MoveUp", "EditorIcons"), BUTTON_MOVE_UP);
		mixin_item->add_button(3, Control::get_icon("MoveDown", "EditorIcons"), BUTTON_MOVE_DOWN);
		mixin_item->add_button(3, Control::get_icon("ScriptRemove", "EditorIcons"), BUTTON_REMOVE);

		mixin_item->set_meta("script", mixin);
	}
	if (script->get_mixin_count() > 0) {
		tree_mixins->show();
		panel_mixins->hide();
	} else {
		tree_mixins->hide();
		panel_mixins->show();
	}

	script->_change_notify();
	update_queued = false;
}

void MixinScriptEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update_list"), &MixinScriptEditor::_update_list);
	ClassDB::bind_method(D_METHOD("queue_update"), &MixinScriptEditor::queue_update);

	ClassDB::bind_method(D_METHOD("_on_editor_script_changed"), &MixinScriptEditor::_on_editor_script_changed);

	ClassDB::bind_method(D_METHOD("_on_attach_main_script_pressed"), &MixinScriptEditor::_on_attach_main_script_pressed);
	ClassDB::bind_method(D_METHOD("_on_main_script_created"), &MixinScriptEditor::_on_main_script_created);
	ClassDB::bind_method(D_METHOD("_on_main_script_creation_closed"), &MixinScriptEditor::_on_main_script_creation_closed);
	ClassDB::bind_method(D_METHOD("_on_main_script_button_pressed"), &MixinScriptEditor::_on_main_script_button_pressed);

	ClassDB::bind_method(D_METHOD("_on_add_mixin_pressed"), &MixinScriptEditor::_on_add_mixin_pressed);
	ClassDB::bind_method(D_METHOD("_on_mixin_created"), &MixinScriptEditor::_on_mixin_created);
	ClassDB::bind_method(D_METHOD("_on_mixin_creation_closed"), &MixinScriptEditor::_on_mixin_creation_closed);
	ClassDB::bind_method(D_METHOD("_on_mixin_button_pressed"), &MixinScriptEditor::_on_mixin_button_pressed);
}

MixinScriptEditor::MixinScriptEditor() {
	vbox = memnew(VBoxContainer);
	add_child(vbox);
	vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->set_anchors_and_margins_preset(Control::PRESET_WIDE);

	// Attach main script button.
	attach_main_script_button = memnew(Button);
	vbox->add_child(attach_main_script_button);
	attach_main_script_button->set_text(TTR("Attach Main Script"));
	attach_main_script_button->set_custom_minimum_size(Vector2(150, 30));
	attach_main_script_button->set_h_size_flags(0);
	attach_main_script_button->connect("pressed", this, "_on_attach_main_script_pressed");

	// Main script (only one).
	tree_main_script = memnew(Tree);
	vbox->add_child(tree_main_script);

	// tree_main_script->connect("cell_selected", this, "_on_main_script_selected");
	// tree_main_script->connect("item_edited", this, "_on_main_script_edited");
	tree_main_script->connect("button_pressed", this, "_on_main_script_button_pressed");
	// tree_main_script->connect("item_activated", this, "_on_main_script_activated");

	tree_main_script->set_column_titles_visible(true);
	tree_main_script->set_hide_root(true);
	tree_main_script->set_columns(3);

	tree_main_script->set_column_title(0, "Script");
	tree_main_script->set_column_expand(0, false);
	tree_main_script->set_column_min_width(0, 200);

	tree_main_script->set_column_title(1, "Path");
	tree_main_script->set_column_expand(1, true);

	tree_main_script->set_column_title(2, "Edit");
	tree_main_script->set_column_expand(2, false);
	tree_main_script->set_column_min_width(2, 100);

	tree_main_script->set_custom_minimum_size(Vector2(0, 75));
	tree_main_script->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tree_main_script->hide(); // Hide initially.

	// Main script placeholder.
	panel_main_script = memnew(PanelContainer);
	vbox->add_child(panel_main_script);
	panel_main_script->set_custom_minimum_size(Vector2(0, 75));
	CenterContainer *center_main_script = memnew(CenterContainer);
	Label *label_main_script = memnew(Label);
	label_main_script->set_text(TTR("Press \"Attach Main Script\" button to create new or load existing script."));
	center_main_script->add_child(label_main_script);
	panel_main_script->add_child(center_main_script);

	// Add mixin button.
	add_mixin_button = memnew(Button);
	vbox->add_child(add_mixin_button);
	add_mixin_button->connect("pressed", this, "_on_add_mixin_pressed");
	add_mixin_button->set_text(TTR("Add Mixin"));
	add_mixin_button->set_tooltip(TTR("Create a partial Script to extend the runtime functionality of the host object."));
	add_mixin_button->set_custom_minimum_size(Vector2(150, 30));
	add_mixin_button->set_h_size_flags(0);

	// A list of mixins.
	tree_mixins = memnew(Tree);
	vbox->add_child(tree_mixins);

	// tree_mixins->connect("cell_selected", this, "_on_mixin_selected");
	// tree_mixins->connect("item_edited", this, "_on_mixin_edited");
	tree_mixins->connect("button_pressed", this, "_on_mixin_button_pressed");
	// tree_mixins->connect("item_activated", this, "_on_mixin_activated");

	tree_mixins->set_column_titles_visible(true);
	tree_mixins->set_hide_root(true);
	tree_mixins->set_columns(4);

	tree_mixins->set_column_title(0, "Order");
	tree_mixins->set_column_expand(0, false);
	tree_mixins->set_column_min_width(0, 50);

	tree_mixins->set_column_title(1, "Script");
	tree_mixins->set_column_expand(1, false);
	tree_mixins->set_column_min_width(1, 200);

	tree_mixins->set_column_title(2, "Path");
	tree_mixins->set_column_expand(2, true);

	tree_mixins->set_column_title(3, "Edit");
	tree_mixins->set_column_expand(3, false);
	tree_mixins->set_column_min_width(3, 100);

	tree_mixins->set_custom_minimum_size(Vector2(0, 75));
	tree_mixins->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tree_mixins->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tree_mixins->hide(); // Hide initially.

	// Mixins placeholder.
	panel_mixins = memnew(PanelContainer);
	vbox->add_child(panel_mixins);
	panel_mixins->set_custom_minimum_size(Vector2(0, 75));
	panel_mixins->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	panel_mixins->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	CenterContainer *center_mixins = memnew(CenterContainer);
	Label *label_mixins = memnew(Label);
	label_mixins->set_text(TTR("Press \"Add Mixin\" button to create or load mixins."));
	center_mixins->add_child(label_mixins);
	panel_mixins->add_child(center_mixins);
}

MixinScriptEditor::~MixinScriptEditor() {
}

void MixinScriptEditor::validate() {
}
