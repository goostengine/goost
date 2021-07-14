#include "git_api.h"

#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/editor_file_system.h"
#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "editor/plugins/version_control_editor_plugin.h"

EditorVCSInterfaceGit *EditorVCSInterfaceGit::singleton = nullptr;

void EditorVCSInterfaceGit::_commit(const String p_msg) {
	int error = 0;

	git_oid tree_id, commit_id;
	git_tree *tree = nullptr;
	git_index *index = nullptr;
	git_object *parent = nullptr;
	git_reference *ref = nullptr;
	git_signature *signature = nullptr;

	String name = EDITOR_GET("version_control/git/user/name");
	String email = EDITOR_GET("version_control/git/user/email");

	if (!name.empty() && !email.empty()) {
		if (name.find("<") >= 0 || name.find(">") >= 0 || email.find("<") >= 0 || email.find(">") >= 0) {
			EditorNode::get_singleton()->show_warning(
					TTR("Invalid Git \"user.name\" or \"user.email\": Should not contain '<' and '>' characters. Aborting commit."));
			return;
		}
		CharString name_utf8 = name.utf8();
		CharString email_utf8 = email.utf8();
		GIT2_CALL(git_signature_now(&signature, name_utf8.get_data(), email_utf8.get_data()), "Could not create signature", nullptr);
	} else {
		// Username and email are not overridden via editor settings, use global config.
		error = git_signature_default(&signature, repo);
		if (error == GIT_ENOTFOUND) {
			EditorNode::get_singleton()->show_warning(
					TTR("No username or email is set in \"Editor Settings/Version Control/Git\".\nConfigure \"user.name\" and \"user.email\" via Editor Settings or Git CLI before committing."));
			return;
		}
	}

	error = git_revparse_ext(&parent, &ref, repo, "HEAD");
	if (error == GIT_ENOTFOUND) {
		// That's alright, we'll be creating initial commit then.
		error = 0;
	}

	GIT2_CALL(git_repository_index(&index, repo), "Could not get repository index", nullptr);
	GIT2_CALL(git_index_write_tree(&tree_id, index), "Could not write index to tree", nullptr);
	GIT2_CALL(git_index_write(index), "Could not write index to disk", nullptr);
	GIT2_CALL(git_tree_lookup(&tree, repo, &tree_id), "Could not lookup tree from ID", nullptr);

	CharString msg = p_msg.utf8();

	GIT2_CALL(
			git_commit_create_v(
					&commit_id,
					repo,
					"HEAD",
					signature,
					signature,
					"UTF-8",
					msg.get_data(),
					tree,
					parent ? 1 : 0, parent,
					parent),
			"Could not create a commit",
			nullptr);

	git_index_free(index);
	git_signature_free(signature);
	git_tree_free(tree);
}

void EditorVCSInterfaceGit::_stage_file(const String p_file_path) {
	git_index *index;
	CharString fp = p_file_path.utf8();
	char *paths[] = { (char *)fp.get_data() };
	git_strarray array = { paths, 1 };

	GIT2_CALL(git_repository_index(&index, repo), "Could not get repository index", nullptr);
	GIT2_CALL(git_index_add_all(index, &array, GIT_INDEX_ADD_DEFAULT | GIT_INDEX_ADD_DISABLE_PATHSPEC_MATCH, nullptr, nullptr), "Could not add a file", nullptr);
	GIT2_CALL(git_index_write(index), "Could not write changes to disk", nullptr);

	git_index_free(index);
}

void EditorVCSInterfaceGit::_unstage_file(const String p_file_path) {
	CharString fp = p_file_path.utf8();
	char *paths[] = { (char *)fp.get_data() };
	git_strarray array = { paths, 1 };

	git_reference *head;
	git_object *head_commit;

	git_repository_head(&head, repo);
	git_reference_peel(&head_commit, head, GIT_OBJ_COMMIT);

	git_reset_default(repo, head_commit, &array);
}

void EditorVCSInterfaceGit::create_gitignore_and_gitattributes() {
	// clang-format off
	if (!FileAccess::exists("res://.gitignore")) {
		// Sync with: https://github.com/github/gitignore/blob/master/Godot.gitignore
		FileAccessRef file = FileAccess::open("res://.gitignore", FileAccess::WRITE);
		file->store_string(
R"(# Godot-specific ignores
.import/
export.cfg
export_presets.cfg

# Imported translations (automatically generated from CSV files)
*.translation

# Mono-specific ignores
.mono/
data_*/
)");
		file->close();
	}

	if (!FileAccess::exists("res://.gitattributes")) {
		FileAccessRef file = FileAccess::open("res://.gitattributes", FileAccess::WRITE);
		file->store_string(
R"(# Normalize EOL for all files that Git considers text files.
* text=auto eol=lf

# Explicitly declare text files you want to always be normalized and converted
# to native line endings on checkout.
*.gd text
*.cs text
*.cpp text
*.c text
*.h text

# Declare files that will always have CRLF line endings on checkout.
*.sln text eol=crlf

# Denote all files that are truly binary and should not be modified.
*.png binary
*.jpg binary
*.hdr binary
)");
		file->close();
	}
	// clang-format on
}

bool EditorVCSInterfaceGit::_is_vcs_initialized() {
	return is_initialized;
}

Dictionary EditorVCSInterfaceGit::_get_modified_files_data() {
	git_status_options opts = GIT_STATUS_OPTIONS_INIT;
	opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	opts.flags = GIT_STATUS_OPT_EXCLUDE_SUBMODULES;
	opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

	git_status_list *statuses = nullptr;
	GIT2_CALL(git_status_list_new(&statuses, repo, &opts), "Could not get status information from the repository", nullptr);

	Dictionary diff; // Schema is <file_path, status>
	size_t count = git_status_list_entrycount(statuses);
	for (size_t i = 0; i < count; ++i) {
		const git_status_entry *entry = git_status_byindex(statuses, i);
		String path;
		if (entry->index_to_workdir) {
			path = entry->index_to_workdir->new_file.path;
		} else {
			path = entry->head_to_index->new_file.path;
		}
		switch (entry->status) {
			case GIT_STATUS_INDEX_NEW:
			case GIT_STATUS_WT_NEW: {
				diff[path] = 0;
			} break;
			case GIT_STATUS_INDEX_MODIFIED:
			case GIT_STATUS_WT_MODIFIED: {
				diff[path] = 1;
			} break;
			case GIT_STATUS_INDEX_RENAMED:
			case GIT_STATUS_WT_RENAMED: {
				diff[path] = 2;
			} break;
			case GIT_STATUS_INDEX_DELETED:
			case GIT_STATUS_WT_DELETED: {
				diff[path] = 3;
			} break;
			case GIT_STATUS_INDEX_TYPECHANGE:
			case GIT_STATUS_WT_TYPECHANGE: {
				diff[path] = 4;
			} break;
			default: {
				// Not handled, or not currently implemented.
			}
		}
	}
	git_status_list_free(statuses);

	return diff;
}

Array EditorVCSInterfaceGit::_get_file_diff(const String file_path) {
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	git_diff *diff;
	CharString file_path_utf8 = file_path.utf8();
	char *pathspec = (char *)file_path_utf8.get_data();

	opts.context_lines = 3;
	opts.interhunk_lines = 0;
	opts.flags = GIT_DIFF_DISABLE_PATHSPEC_MATCH | GIT_DIFF_INCLUDE_UNTRACKED;
	opts.pathspec.strings = &pathspec;
	opts.pathspec.count = 1;

	GIT2_CALL(git_diff_index_to_workdir(&diff, repo, nullptr, &opts), "Could not create diff for index from the working directory", nullptr);

	diff_contents.clear();
	GIT2_CALL(git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, diff_line_callback_function, nullptr), "Call to diff handler provided unsuccessful", nullptr);

	git_diff_free(diff);

	return diff_contents;
}

String EditorVCSInterfaceGit::_get_project_name() {
	return String("project");
}

String EditorVCSInterfaceGit::_get_vcs_name() {
	return "Git";
}

bool EditorVCSInterfaceGit::_initialize(const String p_project_root_path) {
	ERR_FAIL_COND_V(p_project_root_path == "", false);

	singleton = this;

	int init = git_libgit2_init();
	if (init > 1) {
		WARN_PRINT("Multiple libgit2 instances are running");
	}
	if (repo) {
		return true;
	}

	CharString project_root_path_utf8 = p_project_root_path.utf8();

	GIT2_CALL(git_repository_init(&repo, project_root_path_utf8.get_data(), 0), "Could not initialize repository", nullptr);

	if (git_repository_head_unborn(repo) == 1) {
		create_gitignore_and_gitattributes();
	}
	GIT2_CALL(git_repository_open(&repo, project_root_path_utf8.get_data()), "Could not open a repository", nullptr);
	is_initialized = true;

	return is_initialized;
}

bool EditorVCSInterfaceGit::_shut_down() {
	git_repository_free(repo);
	GIT2_CALL(git_libgit2_shutdown(), "Could not shutdown Git plugin", nullptr);
	is_initialized = false;
	return true;
}

// Manager

void EditorVCSInterfaceGitManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_WM_FOCUS_IN: {
			if (!DirAccess::exists(".git") && EditorVCSInterface::get_singleton()) {
				_shutdown();
				WARN_PRINT("Git plugin shutdown: no valid repository is found.");
			}
			if (EditorVCSInterface::get_singleton()) {
				VersionControlEditorPlugin::get_singleton()->call_deferred("_refresh_stage_area");
				vcs_popup->set_item_text(vcs_popup->get_item_index(OPTION_SETUP_SHUTDOWN_REPOSITORY), TTR("Shut Down Git Plugin"));
			} else {
				if (!DirAccess::exists(".git")) {
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

		if (!DirAccess::exists(".git")) {
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
