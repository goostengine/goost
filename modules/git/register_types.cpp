#include "register_types.h"

#include "core/os/os.h"
#include "editor/editor_node.h"
#include "editor/plugins/version_control_editor_plugin.h"

#include "git_api.h"

// We use a special object which manages Git VCS instance to handle notifications,
// since `EditorVCSInterfaceGit` may not be yet initialized or shut down.
//
// We also don't rely on Godot's own VCS manager because it uses script instances
// over object instances to drive VCS plugins.
EditorVCSInterfaceGitManager *git_manager = nullptr;

static void add_initialize_git_button() {
	ERR_FAIL_NULL_MSG(git_manager, "Git manager is not instantiated");

	// Use https://github.com/Zylann/godot_editor_debugger_plugin if this breaks at some point.
	auto *gui_base = EditorNode::get_singleton()->get_gui_base();
	auto *main_vbox = Object::cast_to<VBoxContainer>(gui_base->get_child(1));
	ERR_FAIL_NULL_MSG(main_vbox, "Goost: Could not find main VBoxContainer.");
	auto *menu_hb = Object::cast_to<HBoxContainer>(main_vbox->get_child(0));
	auto *left_menu_hb = Object::cast_to<HBoxContainer>(menu_hb->get_child(0));

	bool found_vcs_popup = false;
	for (int i = 0; i < left_menu_hb->get_child_count(); ++i) {
		MenuButton *mb = Object::cast_to<MenuButton>(left_menu_hb->get_child(i));
		
		if (mb && mb->get_text() == TTR("Project")) {
			PopupMenu *p = mb->get_popup();

			for (int j = 0; j < p->get_item_count(); ++j) {
				if (p->get_item_text(j) == TTR("Version Control")) {
					// See godot/editor/editor_node.cpp for exact node name.
					auto *vc = Object::cast_to<PopupMenu>(p->get_node(NodePath("Version Control")));
					ERR_FAIL_NULL(vc);
					vc->add_separator(TTR("Built-in"));
					Ref<Texture> git_icon = gui_base->get_icon("Git", "EditorIcons");
					if (!DirAccess::exists(".git")) {
						vc->add_icon_item(git_icon, TTR("Set Up Git Repository"), EditorVCSInterfaceGitManager::OPTION_SETUP_SHUTDOWN_REPOSITORY);
					} else {
						if (bool(EDITOR_GET("version_control/git/initialize_plugin_at_editor_startup"))) {
							vc->add_icon_item(git_icon, TTR("Shut Down Git Plugin"), EditorVCSInterfaceGitManager::OPTION_SETUP_SHUTDOWN_REPOSITORY);
						} else {
							vc->add_icon_item(git_icon, TTR("Set Up Git Plugin"), EditorVCSInterfaceGitManager::OPTION_SETUP_SHUTDOWN_REPOSITORY);
						}
					}
					vc->connect("id_pressed", git_manager, "_project_menu_option_pressed", varray(vc));
					found_vcs_popup = true;
					break;
				}
			}
		}
		if (found_vcs_popup) {
			break;
		}
	}
}

static void setup_git() {
	ERR_FAIL_NULL_MSG(git_manager, "Git manager is not instantiated");

	EDITOR_DEF("version_control/git/initialize_plugin_at_editor_startup", true);

	if (DirAccess::exists(".git") && bool(EDITOR_GET("version_control/git/initialize_plugin_at_editor_startup"))) {
		// This is hacky, but required to prevent crash when `VersionControlEditorPlugin`
		// is added to the right dock before editor GUI is ready. Normally, this
		// wouldn't lead to crash because a user is supposed to click on the
		// "Initialize VCS" menu option in the editor, but since we're forcefully
		// enabling the Git addon, we must defer this up until editor GUI is initialized.
		EditorNode::get_singleton()->get_gui_base()->connect("ready", git_manager, "_setup");
	}
}

void register_git_types() {
	ClassDB::set_current_api(ClassDB::APIType::API_EDITOR);
	ClassDB::register_class<EditorVCSInterfaceGit>();
	ClassDB::set_current_api(ClassDB::API_CORE);

	git_manager = memnew(EditorVCSInterfaceGitManager);

	// Order matters.
	EditorNode::add_init_callback(setup_git);
	EditorNode::add_init_callback(add_initialize_git_button);
}

void unregister_git_types() {
	if (EditorVCSInterface::get_singleton()) {
		EditorVCSInterface::get_singleton()->shut_down();
	}
	// Validate before attempting delete, the interface may get deleted
	// either by `VersionControlEditorPlugin` or here...
	if (ObjectDB::instance_validate(EditorVCSInterface::get_singleton())) {
		memdelete(EditorVCSInterface::get_singleton());
	}
	EditorVCSInterface::set_singleton(nullptr);

	if (git_manager) {
		memdelete(git_manager);
	}
}
