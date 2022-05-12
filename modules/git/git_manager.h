#pragma once

#include "git_api.h"

#include "editor/editor_vcs_interface.h"
#include "editor/plugins/version_control_editor_plugin.h"

class EditorVCSInterfaceGitManager : public Node {
	GDCLASS(EditorVCSInterfaceGitManager, Node);

	PopupMenu *vcs_popup = nullptr;

	void _project_menu_option_pressed(int p_idx, Object *p_menu);
	bool _setup();
	void _shutdown();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	enum {
		OPTION_SETUP_SHUTDOWN_REPOSITORY = 9000,
	};
	void set_popup_menu(PopupMenu *p_popup) { vcs_popup = p_popup; }
	PopupMenu *get_popup_menu(PopupMenu *p_popup) { return vcs_popup; }

	static bool repository_exists();
};
