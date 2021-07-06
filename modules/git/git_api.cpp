#include "git_api.h"

#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/plugins/version_control_editor_plugin.h"
#include "editor/editor_file_system.h"

EditorVCSInterfaceGit *EditorVCSInterfaceGit::singleton = nullptr;

void EditorVCSInterfaceGit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_setup"), &EditorVCSInterfaceGit::_setup);
}

void EditorVCSInterfaceGit::_setup() {
	// It may happen that there are other VCS currently registered, prevent potentially memory leak.
	if (EditorVCSInterface::get_singleton()) {
		memdelete(EditorVCSInterface::get_singleton());
	}
	// EditorVCSInterfaceGit singleton must NOT be assigned until `register_editor` is called.
	// otherwise no docks will appear.
	VersionControlEditorPlugin::get_singleton()->register_editor();

	// Assign EditorVCSInterfaceGit singleton now, after editor is registered.
	EditorVCSInterface::set_singleton(EditorVCSInterfaceGit::get_singleton());

    EditorFileSystem::get_singleton()->connect("filesystem_changed", VersionControlEditorPlugin::get_singleton(), "_refresh_stage_area");

	String res_dir = OS::get_singleton()->get_resource_dir();
	bool vcs_initialized = EditorVCSInterface::get_singleton()->initialize(res_dir);
	ERR_FAIL_COND_MSG(!vcs_initialized, "VCS was not initialized");
}

void EditorVCSInterfaceGit::_commit(const String p_msg) {
	if (!can_commit) {
		ERR_FAIL_MSG("Git API cannot commit. Check previous errors.");
		return;
	}
	git_signature *default_sign;
	git_oid tree_id, parent_commit_id, new_commit_id;
	git_tree *tree;
	git_index *repo_index;
	git_commit *parent_commit;

	GIT2_CALL(git_repository_index(&repo_index, repo), "Could not get repository index", NULL);
	for (int i = 0; i < staged_files.size(); i++) {
		String file_path = staged_files[i];
		CharString file_path_utf8 = file_path.utf8();
		if (FileAccess::exists(file_path)) {
			GIT2_CALL(git_index_add_bypath(repo_index, file_path_utf8.get_data()), "Could not add file by path", NULL);
		} else {
			GIT2_CALL(git_index_remove_bypath(repo_index, file_path_utf8.get_data()), "Could not add file by path", NULL);
		}
	}
	GIT2_CALL(git_index_write_tree(&tree_id, repo_index), "Could not write index to tree", NULL);
	GIT2_CALL(git_index_write(repo_index), "Could not write index to disk", NULL);
	GIT2_CALL(git_signature_default(&default_sign, repo), "Could not get default signature", NULL);
	GIT2_CALL(git_tree_lookup(&tree, repo, &tree_id), "Could not lookup tree from ID", NULL);

	GIT2_CALL(git_reference_name_to_id(&parent_commit_id, repo, "HEAD"), "Could not get parent ID", NULL);
	GIT2_CALL(git_commit_lookup(&parent_commit, repo, &parent_commit_id), "Could not lookup parent commit data", NULL);

	CharString msg = p_msg.utf8();

	GIT2_CALL(
			git_commit_create_v(
					&new_commit_id,
					repo,
					"HEAD",
					default_sign,
					default_sign,
					"UTF-8",
					msg.get_data(),
					tree,
					1,
					parent_commit),
			"Could not create commit",
			NULL);

	staged_files.clear();

	git_index_free(repo_index);
	git_signature_free(default_sign);
	git_commit_free(parent_commit);
	git_tree_free(tree);
}

void EditorVCSInterfaceGit::_stage_file(const String p_file_path) {
	if (staged_files.find(p_file_path) == -1) {
		staged_files.push_back(p_file_path);
	}
}

void EditorVCSInterfaceGit::_unstage_file(const String p_file_path) {
	if (staged_files.find(p_file_path) != -1) {
		staged_files.erase(p_file_path);
	}
}

void EditorVCSInterfaceGit::create_gitignore_and_gitattributes() {
	if (!FileAccess::exists("res://.gitignore")) {
		FileAccessRef file = FileAccess::open("res://.gitignore", FileAccess::WRITE);
		file->store_string(
				"# Import cache\n"
				".import/\n\n"

				"# Binaries\n"
				"bin/\n"
				"build/\n"
				"lib/\n");
		file->close();
	}

	if (!FileAccess::exists("res://.gitattributes")) {
		FileAccessRef file = FileAccess::open("res://.gitattributes", FileAccess::WRITE);
		file->store_string(
				"# Set the default behavior, in case people don't have core.autocrlf set.\n"
				"* text=auto\n\n"

				"# Explicitly declare text files you want to always be normalized and converted\n"
				"# to native line endings on checkout.\n"
				"*.cpp text\n"
				"*.c text\n"
				"*.h text\n"
				"*.gd text\n"
				"*.cs text\n\n"

				"# Declare files that will always have CRLF line endings on checkout.\n"
				"*.sln text eol=crlf\n\n"

				"# Denote all files that are truly binary and should not be modified.\n"
				"*.png binary\n"
				"*.jpg binary\n");
		file->close();
	}
}

bool EditorVCSInterfaceGit::create_initial_commit() {
	git_signature *sig;
	git_oid tree_id, commit_id;
	git_index *repo_index;
	git_tree *tree;

	if (git_signature_default(&sig, repo) != 0) {
		ERR_FAIL_V_MSG(false, "Unable to create a commit signature. Perhaps 'user.name' and 'user.email' are not set. Set default user name and user email by `git config` and initialize again");
	}

	GIT2_CALL(git_repository_index(&repo_index, repo), "Could not get repository index", NULL);
	GIT2_CALL(git_index_write_tree(&tree_id, repo_index), "Could not create intial commit", NULL);

	GIT2_CALL(git_tree_lookup(&tree, repo, &tree_id), "Could not create intial commit", NULL);
	GIT2_CALL(
			git_commit_create_v(
					&commit_id,
					repo,
					"HEAD",
					sig,
					sig,
					NULL,
					"Initial commit",
					tree,
					0),
			"Could not create the initial commit",
			NULL);

	GIT2_CALL(git_index_write(repo_index), "Could not write index to disk", NULL);
	git_index_free(repo_index);
	git_tree_free(tree);
	git_signature_free(sig);

	return true;
}

bool EditorVCSInterfaceGit::_is_vcs_initialized() {
	return is_initialized;
}

Dictionary EditorVCSInterfaceGit::_get_modified_files_data() {
	git_status_options opts = GIT_STATUS_OPTIONS_INIT;
	opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	opts.flags = GIT_STATUS_OPT_EXCLUDE_SUBMODULES;
	opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

	git_status_list *statuses = NULL;
	GIT2_CALL(git_status_list_new(&statuses, repo, &opts), "Could not get status information from repository", NULL);

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

	GIT2_CALL(git_diff_index_to_workdir(&diff, repo, NULL, &opts), "Could not create diff for index from working directory", NULL);

	diff_contents.clear();
	GIT2_CALL(git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, diff_line_callback_function, NULL), "Call to diff handler provided unsuccessful", NULL);

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

	can_commit = true;
	GIT2_CALL(git_repository_init(&repo, project_root_path_utf8.get_data(), 0), "Could not initialize repository", NULL);

	if (git_repository_head_unborn(repo) == 1) {
		create_gitignore_and_gitattributes();
		if (!create_initial_commit()) {
			ERR_PRINT("Initial commit could not be created. Commit functionality will not work.");
			can_commit = false;
		}
	}
	GIT2_CALL(git_repository_open(&repo, project_root_path_utf8.get_data()), "Could not open repository", NULL);
	is_initialized = true;

	return is_initialized;
}

bool EditorVCSInterfaceGit::_shut_down() {
	git_repository_free(repo);
	GIT2_CALL(git_libgit2_shutdown(), "Could not shutdown Git Addon", NULL);
	return true;
}
