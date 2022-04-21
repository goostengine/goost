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
		GIT2_CALL(git_signature_now(&signature, name_utf8.get_data(), email_utf8.get_data()), "Could not create signature");
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

	GIT2_CALL(git_repository_index(&index, repo), "Could not get repository index");
	GIT2_CALL(git_index_write_tree(&tree_id, index), "Could not write index to tree");
	GIT2_CALL(git_index_write(index), "Could not write index to disk");
	GIT2_CALL(git_tree_lookup(&tree, repo, &tree_id), "Could not lookup tree from ID");

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
			"Could not create a commit");

	git_index_free(index);
	git_signature_free(signature);
	git_tree_free(tree);
}

void EditorVCSInterfaceGit::_stage_file(const String p_file_path) {
	git_index *index;
	CharString fp = p_file_path.utf8();
	char *paths[] = { (char *)fp.get_data() };
	git_strarray array = { paths, 1 };

	GIT2_CALL(git_repository_index(&index, repo), "Could not get repository index");
	GIT2_CALL(git_index_add_all(index, &array, GIT_INDEX_ADD_DEFAULT | GIT_INDEX_ADD_DISABLE_PATHSPEC_MATCH, nullptr, nullptr), "Could not add a file");
	GIT2_CALL(git_index_write(index), "Could not write changes to disk");

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

Array EditorVCSInterfaceGit::_get_modified_files_data() {
	Array stats_files;

	git_status_options opts = GIT_STATUS_OPTIONS_INIT;
	opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	opts.flags = GIT_STATUS_OPT_EXCLUDE_SUBMODULES;
	opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

	git_status_list *statuses = nullptr;
	GIT2_CALL_V(git_status_list_new(&statuses, repo, &opts), "Could not get status information from repository", stats_files);

	size_t count = git_status_list_entrycount(statuses);
	for (size_t i = 0; i < count; ++i) {
		const git_status_entry *entry = git_status_byindex(statuses, i);
		String path;
		if (entry->index_to_workdir) {
			path = entry->index_to_workdir->new_file.path;
		} else {
			path = entry->head_to_index->new_file.path;
		}
		static Map<int, ChangeType> map_changes;
		map_changes[GIT_STATUS_WT_NEW] = CHANGE_TYPE_NEW;
		map_changes[GIT_STATUS_INDEX_NEW] = CHANGE_TYPE_NEW;
		map_changes[GIT_STATUS_WT_MODIFIED] = CHANGE_TYPE_MODIFIED;
		map_changes[GIT_STATUS_INDEX_MODIFIED] = CHANGE_TYPE_MODIFIED;
		map_changes[GIT_STATUS_WT_RENAMED] = CHANGE_TYPE_RENAMED;
		map_changes[GIT_STATUS_INDEX_RENAMED] = CHANGE_TYPE_RENAMED;
		map_changes[GIT_STATUS_WT_DELETED] = CHANGE_TYPE_DELETED;
		map_changes[GIT_STATUS_INDEX_DELETED] = CHANGE_TYPE_DELETED;
		map_changes[GIT_STATUS_WT_TYPECHANGE] = CHANGE_TYPE_TYPECHANGE;
		map_changes[GIT_STATUS_INDEX_TYPECHANGE] = CHANGE_TYPE_TYPECHANGE;
		map_changes[GIT_STATUS_CONFLICTED] = CHANGE_TYPE_UNMERGED;

		const static int git_status_wt = GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED | GIT_STATUS_CONFLICTED;
		const static int git_status_index = GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE;

		if (entry->status & git_status_wt) {
			stats_files.push_back(create_status_file(path, map_changes[entry->status & git_status_wt], TREE_AREA_UNSTAGED));
		}
		if (entry->status & git_status_index) {
			if (entry->status == GIT_STATUS_INDEX_RENAMED) {
				String old_path = entry->head_to_index->old_file.path;
				stats_files.push_back(create_status_file(old_path, map_changes[GIT_STATUS_INDEX_DELETED], TREE_AREA_STAGED));
				stats_files.push_back(create_status_file(path, map_changes[GIT_STATUS_INDEX_NEW], TREE_AREA_STAGED));
			} else {
				stats_files.push_back(create_status_file(path, map_changes[entry->status & git_status_index], TREE_AREA_STAGED));
			}
		}
	}
	git_status_list_free(statuses);

	return stats_files;
}

Array EditorVCSInterfaceGit::_get_file_diff(const String file_path, int area) {
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	git_diff *diff;
	CharString file_path_utf8 = file_path.utf8();
	char *pathspec = (char *)file_path_utf8.get_data();

	opts.context_lines = 2;
	opts.interhunk_lines = 0;
	opts.flags = GIT_DIFF_DISABLE_PATHSPEC_MATCH | GIT_DIFF_INCLUDE_UNTRACKED;
	opts.pathspec.strings = &pathspec;
	opts.pathspec.count = 1;

	switch ((TreeArea)area) {
		case TREE_AREA_UNSTAGED: {
			GIT2_CALL_V(git_diff_index_to_workdir(&diff, repo, NULL, &opts), "Could not create diff for index from working directory", diff_contents);
		} break;
		case TREE_AREA_STAGED: {
			git_object *obj = nullptr;
			git_tree *tree = nullptr;

			GIT2_CALL_V(git_revparse_single(&obj, repo, "HEAD^{tree}"), "", diff_contents);
			GIT2_CALL_V(git_tree_lookup(&tree, repo, git_object_id(obj)), "", diff_contents);
			GIT2_CALL_V(git_diff_tree_to_index(&diff, repo, tree, NULL, &opts), "Could not create diff for tree from index directory", diff_contents);

			git_tree_free(tree);
			git_object_free(obj);
		} break;
		case TREE_AREA_COMMIT: {
			opts.pathspec = {};
			git_object *obj = nullptr;
			git_commit *commit = nullptr, *parent = nullptr;
			git_tree *commit_tree = nullptr, *parent_tree = nullptr;

			GIT2_CALL_V(git_revparse_single(&obj, repo, pathspec), "", diff_contents);
			GIT2_CALL_V(git_commit_lookup(&commit, repo, git_object_id(obj)), "", diff_contents);
			GIT2_CALL_V(git_commit_parent(&parent, commit, 0), "", diff_contents);
			GIT2_CALL_V(git_commit_tree(&commit_tree, commit), "", diff_contents);
			GIT2_CALL_V(git_commit_tree(&parent_tree, parent), "", diff_contents);
			GIT2_CALL_V(git_diff_tree_to_tree(&diff, repo, parent_tree, commit_tree, &opts), "", diff_contents);

			git_object_free(obj);
			git_commit_free(commit);
			git_commit_free(parent);
			git_tree_free(commit_tree);
			git_tree_free(parent_tree);
		} break;
	}
	diff_contents = _parse_diff(diff);
	git_diff_free(diff);

	return diff_contents;
}

Array EditorVCSInterfaceGit::_parse_diff(git_diff *diff) {
	Array diff_contents;
	for (size_t i = 0; i < git_diff_num_deltas(diff); i++) {
		git_patch *patch;
		const git_diff_delta *delta = git_diff_get_delta(diff, i); // file_cb
		git_patch_from_diff(&patch, diff, i);

		if (delta->flags & GIT_DIFF_FLAG_BINARY) {
			continue;
		}

		if (delta->status == GIT_DELTA_UNMODIFIED || delta->status == GIT_DELTA_IGNORED || delta->status == GIT_DELTA_UNTRACKED) {
			continue;
		}
		Dictionary diff_file = create_diff_file(delta->new_file.path, delta->old_file.path);

		Array diff_hunks;
		for (size_t j = 0; j < git_patch_num_hunks(patch); j++) {
			const git_diff_hunk *git_hunk;
			size_t line_count;
			git_patch_get_hunk(&git_hunk, &line_count, patch, j);

			Dictionary diff_hunk = create_diff_hunk(git_hunk->old_start, git_hunk->new_start, git_hunk->old_lines, git_hunk->new_lines);

			Array diff_lines;
			for (size_t k = 0; k < line_count; k++) {
				const git_diff_line *git_diff_line;
				git_patch_get_line_in_hunk(&git_diff_line, patch, j, k); // line_cb
				char *content = new char[git_diff_line->content_len + 1];
				memcpy(content, git_diff_line->content, git_diff_line->content_len);
				content[git_diff_line->content_len] = '\0';

				Dictionary diff_line = create_diff_line(git_diff_line->new_lineno, git_diff_line->old_lineno, String((const char *)content), vformat("%s", git_diff_line->origin));
				diff_lines.push_back(diff_line);
			}

			diff_hunk = add_line_diffs_into_diff_hunk(diff_hunk, diff_lines);
			diff_hunks.push_back(diff_hunk);
		}
		diff_file = add_diff_hunks_into_diff_file(diff_file, diff_hunks);
		diff_contents.push_back(diff_file);

		git_patch_free(patch);
	}
	return diff_contents;
}

Array EditorVCSInterfaceGit::_get_line_diff(String p_file_path, String p_text) {
	git_index *index;
	git_blob *blob;
	git_reference *head;

	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	// git_diff *diff; // Unused: intended?
	Array diff_contents;

	opts.context_lines = 0;
	opts.flags = GIT_DIFF_DISABLE_PATHSPEC_MATCH | GIT_DIFF_INCLUDE_UNTRACKED;

	GIT2_CALL_V(git_repository_index(&index, repo), "Failed to get repository index", diff_contents);
	GIT2_CALL_V(git_index_read(index, 0), "Failed to read index", diff_contents);

	CharString file_path_utf8 = p_file_path.utf8();
	char *filepath = (char *)file_path_utf8.get_data();

	const git_index_entry *entry = git_index_get_bypath(index, filepath, GIT_INDEX_STAGE_NORMAL);
	if (!entry) {
		git_index_free(index);
		return diff_contents;
	}
	const git_oid *blobSha = &entry->id;
	GIT2_CALL_V(git_repository_head(&head, repo), "Failed to load repository head", diff_contents);
	GIT2_CALL_V(git_blob_lookup(&blob, repo, blobSha), "Failed to load head blob", diff_contents);

	CharString text_utf8 = p_text.utf8();
	char *text = (char *)text_utf8.get_data();
	GIT2_CALL_V(git_diff_blob_to_buffer(blob, NULL, text, p_text.length(), NULL, &opts, NULL, NULL, diff_hunk_cb, NULL, &diff_contents), "Failed to make diff", diff_contents);

	git_index_free(index);
	git_blob_free(blob);
	git_reference_free(head);
	// git_diff_free(diff);

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

	String path = GLOBAL_GET("version_control/git/repository_path");
	if (path.empty()) {
		path = p_project_root_path;
	} else if (!DirAccess::exists(path)) {
		WARN_PRINT(vformat("Repository path \"%s\" does not exist, please configure \"version_control/git/repository_path\" project setting.", path));
		path = p_project_root_path;
	}

	CharString repo_path = path.utf8();

	GIT2_CALL_V(git_repository_init(&repo, repo_path.get_data(), 0), "Could not initialize repository", false);

	if (git_repository_head_unborn(repo) == 1) {
		create_gitignore_and_gitattributes();
	}
	GIT2_CALL_V(git_repository_open(&repo, repo_path.get_data()), "Could not open a repository", false);
	is_initialized = true;

	return is_initialized;
}

bool EditorVCSInterfaceGit::_shut_down() {
	git_repository_free(repo);
	GIT2_CALL_V(git_libgit2_shutdown(), "Could not shutdown Git plugin", false);
	is_initialized = false;
	return true;
}

void EditorVCSInterfaceGit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_commit"), &EditorVCSInterfaceGit::_commit);
	ClassDB::bind_method(D_METHOD("_is_vcs_initialized"), &EditorVCSInterfaceGit::_is_vcs_initialized);
	ClassDB::bind_method(D_METHOD("_get_modified_files_data"), &EditorVCSInterfaceGit::_get_modified_files_data);
	ClassDB::bind_method(D_METHOD("_get_file_diff"), &EditorVCSInterfaceGit::_get_file_diff);
	ClassDB::bind_method(D_METHOD("_get_line_diff"), &EditorVCSInterfaceGit::_get_line_diff);
	ClassDB::bind_method(D_METHOD("_get_project_name"), &EditorVCSInterfaceGit::_get_project_name);
	ClassDB::bind_method(D_METHOD("_get_vcs_name"), &EditorVCSInterfaceGit::_get_vcs_name);
	ClassDB::bind_method(D_METHOD("_initialize"), &EditorVCSInterfaceGit::_initialize);
	ClassDB::bind_method(D_METHOD("_shut_down"), &EditorVCSInterfaceGit::_shut_down);
	ClassDB::bind_method(D_METHOD("_stage_file"), &EditorVCSInterfaceGit::_stage_file);
	ClassDB::bind_method(D_METHOD("_unstage_file"), &EditorVCSInterfaceGit::_unstage_file);
}

// Manager

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
