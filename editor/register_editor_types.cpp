#include "register_editor_types.h"

#include "editor/editor_node.h"
#include "editor_about.h"

namespace goost {

void editor_init() {
	auto about = memnew(EditorAbout);

	auto *gui_base = EditorNode::get_singleton()->get_gui_base();
	gui_base->add_child(about);

	// Use https://github.com/Zylann/godot_editor_debugger_plugin if this breaks at some point.
	auto *main_vbox = Object::cast_to<VBoxContainer>(gui_base->get_child(1));
	ERR_FAIL_NULL_MSG(main_vbox, "Goost: Could not find main VBoxContainer.");
	auto *menu_hb = Object::cast_to<HBoxContainer>(main_vbox->get_child(0));
	auto *left_menu_hb = Object::cast_to<HBoxContainer>(menu_hb->get_child(0));

	for (int i = 0; i < left_menu_hb->get_child_count(); ++i) {
		MenuButton *mb = Object::cast_to<MenuButton>(left_menu_hb->get_child(i));
		if (mb && mb->get_text() == TTR("Help")) {
			PopupMenu *p = mb->get_popup();
			p->add_separator();
			p->add_icon_item(gui_base->get_icon("GoostEngine", "EditorIcons"), TTR("About Goost"), EditorAbout::HELP_ABOUT);
			p->connect("id_pressed", about, "_help_menu_pressed");
			break;
		}
	}
}

void register_editor_types() {
	EditorNode::add_init_callback(editor_init);
}

void unregister_editor_types() {
	// Nothing to do yet.
}

} // namespace goost
