#include "register_types.h"

#include "core/os/os.h"
#include "editor/editor_node.h"
#include "editor/plugins/version_control_editor_plugin.h"

#include "git_api.h"

EditorVCSInterface *vcs_interface = nullptr;

static void initialize_git() {
	ERR_FAIL_NULL(vcs_interface);

	// This is hacky, but required to prevent crash when `VersionControlEditorPlugin`
	// is added to the right dock before editor GUI is ready. Normally, this
	// wouldn't lead to crash because a user is supposed to click on the
	// "Initialize VCS" menu option in the editor, but since we're forcefully
	// enabling the Git addon, we must defer this up until editor GUI is initialized.
	EditorNode::get_singleton()->get_gui_base()->connect("ready", vcs_interface, "_setup");
}

void register_git_types() {
	ClassDB::set_current_api(ClassDB::APIType::API_EDITOR);
	ClassDB::register_class<EditorVCSInterfaceGit>();
	ClassDB::set_current_api(ClassDB::API_CORE);

	vcs_interface = memnew(EditorVCSInterfaceGit);

	EditorNode::add_init_callback(initialize_git);
}

void unregister_git_types() {
	if (vcs_interface) {
		if (EditorVCSInterface::get_singleton()) {
			EditorVCSInterface::get_singleton()->shut_down();
		}
		// Validate before attempting delete, the interface may get deleted
		// either by `VersionControlEditorPlugin` or here...
		if (ObjectDB::instance_validate(vcs_interface)) {
			memdelete(vcs_interface);
		}
		EditorVCSInterface::set_singleton(nullptr);
	}
}
