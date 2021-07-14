#ifndef GOOST_GIT_API_H
#define GOOST_GIT_API_H

// Initial implementation based on Godot's GDNative version of the Git plugin
// See https://github.com/godotengine/godot-git-plugin.

#include "editor/editor_vcs_interface.h"
#include "editor/plugins/version_control_editor_plugin.h"

#include "git_common.h"
#include <git2.h>

class EditorVCSInterfaceGit : public EditorVCSInterface {
	GDCLASS(EditorVCSInterfaceGit, EditorVCSInterface);

	static EditorVCSInterfaceGit *singleton;

	git_repository *repo = nullptr;
	bool is_initialized;

	virtual void _commit(const String p_msg);
	virtual bool _is_vcs_initialized();
	virtual Dictionary _get_modified_files_data();
	virtual Array _get_file_diff(const String file_path);
	virtual String _get_project_name();
	virtual String _get_vcs_name();
	virtual bool _initialize(const String p_project_root_path);
	virtual bool _shut_down();
	virtual void _stage_file(const String p_file_path);
	virtual void _unstage_file(const String p_file_path);

public:
	static EditorVCSInterfaceGit *get_singleton() { return singleton; }
	static void set_singleton(EditorVCSInterfaceGit *p_object) { singleton = p_object; }

	Array diff_contents;

	void create_gitignore_and_gitattributes();

	EditorVCSInterfaceGit() {
		if (!singleton) {
			singleton = this;
		}
	}
	virtual ~EditorVCSInterfaceGit(){};
};

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
};

#endif // GOOST_GIT_API_H
