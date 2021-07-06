#ifndef GOOST_GIT_API_H
#define GOOST_GIT_API_H

// Initial implementation based on Godot's GDNative version of the Git plugin
// See https://github.com/godotengine/godot-git-plugin.

#include "editor/editor_vcs_interface.h"
#include "git_common.h"
#include <git2.h>

class EditorVCSInterfaceGit : public EditorVCSInterface {
	GDCLASS(EditorVCSInterfaceGit, EditorVCSInterface);

	static EditorVCSInterfaceGit *singleton;

	bool is_initialized;
	bool can_commit;

	Array staged_files;

	git_repository *repo = nullptr;

	void _commit(const String p_msg);
	bool _is_vcs_initialized();
	Dictionary _get_modified_files_data();
	Array _get_file_diff(const String file_path);
	String _get_project_name();
	String _get_vcs_name();
	bool _initialize(const String p_project_root_path);
	bool _shut_down();
	void _stage_file(const String p_file_path);
	void _unstage_file(const String p_file_path);
	
	void _setup();

protected:
	static void _bind_methods();

public:
	static EditorVCSInterfaceGit *get_singleton() { return singleton; }

	Array diff_contents;

	void create_gitignore_and_gitattributes();
	bool create_initial_commit();
	
	EditorVCSInterfaceGit() {
		if (!singleton) {
			singleton = this;
		}
	}
};

#endif // GOOST_GIT_API_H
