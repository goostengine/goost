#include "git_manager.h"

#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/editor_file_system.h"
#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "editor/plugins/version_control_editor_plugin.h"

void EditorVCSInterfaceGitManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_WM_FOCUS_IN: {
			if (!repository_exists() && EditorVCSInterface::get_singleton()) {
				_shutdown();
				WARN_PRINT("Git plugin shutdown: no valid repository is found.");
			}
			if (EditorVCSInterface::get_singleton()) {
				VersionControlEditorPlugin::get_singleton()->call_deferred("_refresh_stage_area");
				vcs_popup->set_item_text(vcs_popup->get_item_index(OPTION_SETUP_SHUTDOWN_REPOSITORY), TTR("Shut Down Git Plugin"));
			} else {
				if (!repository_exists()) {
					vcs_popup->set_item_text(vcs_popup->get_item_index(OPTION_SETUP_SHUTDOWN_REPOSITORY), TTR("Set Up Git Repository"));
				} else {
					vcs_popup->set_item_text(vcs_popup->get_item_index(OPTION_SETUP_SHUTDOWN_REPOSITORY), TTR("Set Up Git Plugin"));
				}
			}
		} break;
	}
}

void EditorVCSInterfaceGitManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_project_menu_option_pressed"), &EditorVCSInterfaceGitManager::_project_menu_option_pressed);
	ClassDB::bind_method(D_METHOD("_setup"), &EditorVCSInterfaceGitManager::_setup);
	ClassDB::bind_method(D_METHOD("_shutdown"), &EditorVCSInterfaceGitManager::_shutdown);
}

void EditorVCSInterfaceGitManager::_project_menu_option_pressed(int p_id, Object *p_menu) {
	if (p_id != OPTION_SETUP_SHUTDOWN_REPOSITORY) {
		return;
	}
	auto popup = Object::cast_to<PopupMenu>(p_menu);
	ERR_FAIL_NULL(popup);

	if (!EditorVCSInterface::get_singleton()) {
		bool initialized = _setup();
		if (initialized) {
			popup->set_item_text(popup->get_item_index(p_id), TTR("Shut Down Git Plugin"));
		}
	} else {
		_shutdown();

		if (!repository_exists()) {
			popup->set_item_text(popup->get_item_index(p_id), TTR("Set Up Git Repository"));
		} else {
			popup->set_item_text(popup->get_item_index(p_id), TTR("Set Up Git Plugin"));
		}
	}
}

bool EditorVCSInterfaceGitManager::_setup() {
	// It may happen that there are other VCS currently registered, prevent potential memory leak.
	if (EditorVCSInterface::get_singleton()) {
		memdelete(EditorVCSInterface::get_singleton());
	}
	// EditorVCSInterfaceGit singleton must NOT be assigned until `register_editor` is called.
	// otherwise no docks will appear.
	VersionControlEditorPlugin::get_singleton()->register_editor();

	// Assign EditorVCSInterfaceGit singleton now, after editor is registered.
	if (!EditorVCSInterface::get_singleton()) {
		EditorVCSInterfaceGit::set_singleton(memnew(EditorVCSInterfaceGit));
	}
	EditorVCSInterface::set_singleton(EditorVCSInterfaceGit::get_singleton());

	EditorFileSystem::get_singleton()->connect("filesystem_changed", VersionControlEditorPlugin::get_singleton(), "_refresh_stage_area");
	VersionControlEditorPlugin::get_singleton()->call_deferred("_refresh_stage_area");

	String res_dir = OS::get_singleton()->get_resource_dir();
	bool is_vcs_initialized = EditorVCSInterface::get_singleton()->initialize(res_dir);
	if (!is_vcs_initialized) {
		ERR_PRINT("VCS was not initialized");
	}
	return is_vcs_initialized;
}

void EditorVCSInterfaceGitManager::_shutdown() {
	VersionControlEditorPlugin::get_singleton()->shut_down();
}

bool EditorVCSInterfaceGitManager::repository_exists() {
	if (DirAccess::exists(".git")) {
		return true;
	} else {
		String custom_path = GLOBAL_GET("version_control/git/repository_path");
		if (!custom_path.empty() && DirAccess::exists(custom_path)) {
			return true;
		}
	}
	return false;
}
