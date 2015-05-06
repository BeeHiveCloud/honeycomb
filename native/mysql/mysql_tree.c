#include "mysql_tree.h"

int git_mysql_tree_init(git_mysql *mysql){

	MYSQL_BIND bind_buffers[1];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &(mysql->repo);
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	if (mysql_stmt_bind_param(mysql->tree_init, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_init) != 0)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->tree_init) != 0)
		return GIT_ERROR;

	return GIT_OK;
}

int git_mysql_tree_update(git_mysql *mysql, const char *dir, git_oid *oid){

	MYSQL_BIND bind_buffers[3];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &(mysql->repo);
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	// bind the dir passed to the statement
	bind_buffers[1].buffer = dir;
	bind_buffers[1].buffer_length = strlen(dir) ;
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	// bind the oid passed to the statement
	bind_buffers[2].buffer = oid;
	bind_buffers[2].buffer_length = 20;
	bind_buffers[2].length = &bind_buffers[2].buffer_length;
	bind_buffers[2].buffer_type = MYSQL_TYPE_BLOB;

	if (mysql_stmt_bind_param(mysql->tree_update, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_update) != 0)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->tree_update) != 0)
		return GIT_ERROR;

	return GIT_OK;
}


int git_mysql_tree_build(git_mysql *mysql, git_repository *repo, const char *type){

	int error;
	MYSQL_BIND bind_buffers[2];
	MYSQL_BIND result_buffers[3];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &(mysql->repo);
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	// bind type
	bind_buffers[1].buffer = type;
	bind_buffers[1].buffer_length = strlen(type);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_STRING;

	if (mysql_stmt_bind_param(mysql->tree_build, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_build) != 0)
		return GIT_ERROR;

	if (mysql_stmt_store_result(mysql->tree_build) != 0)
		return GIT_ERROR;

	if (mysql_stmt_num_rows(mysql->tree_build) > 0){

		memset(result_buffers, 0, sizeof(result_buffers));

		git_oid oid;
		char *dir;
		char *entry;
		unsigned long dir_len;
		unsigned long entry_len;

		result_buffers[0].buffer_type = MYSQL_TYPE_BLOB;
		result_buffers[0].buffer = &oid;
		result_buffers[0].buffer_length = 20;
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;
		memset(&oid, 0, 20);

		result_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[1].buffer = 0;
		result_buffers[1].buffer_length = 0;
		result_buffers[1].is_null = 0;
		result_buffers[1].length = &dir_len;

		result_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[2].buffer = 0;
		result_buffers[2].buffer_length = 0;
		result_buffers[2].is_null = 0;
		result_buffers[2].length = &entry_len;

		if (mysql_stmt_bind_result(mysql->tree_build, result_buffers) != 0)
			return GIT_ERROR;

		git_oid tree;
		git_treebuilder   *bld = NULL;
		git_treebuilder_new(&bld, repo, NULL);

		char *curr_dir = "";

		while (mysql_stmt_fetch(mysql->tree_build) == MYSQL_DATA_TRUNCATED){
	
			dir = malloc(dir_len + 1);
			result_buffers[1].buffer = dir;
			result_buffers[1].buffer_length = dir_len + 1;
			if (mysql_stmt_fetch_column(mysql->tree_build, &result_buffers[1], 1, 0) != 0)
				return GIT_ERROR;

			entry = malloc(entry_len + 1);
			result_buffers[2].buffer = entry;
			result_buffers[2].buffer_length = entry_len + 1;
			if (mysql_stmt_fetch_column(mysql->tree_build, &result_buffers[2], 2, 0) != 0)
				return GIT_ERROR;

			if (strcmp(curr_dir, dir)){ // dir change
				if (strlen(curr_dir)){
					error = git_treebuilder_write(&tree, bld);
					git_mysql_tree_update(mysql, curr_dir, &tree);
				}
				git_treebuilder_clear(bld);
				strcpy(curr_dir, dir);
			}

			if (!strcmp(type, "BLOB"))
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_BLOB);
			else
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_TREE);

			free(dir);
			free(entry);

		} ;

		error = git_treebuilder_write(&tree, bld);
		git_mysql_tree_update(mysql, curr_dir, &tree);
		git_treebuilder_free(bld);
		memset(curr_dir, 0, strlen(curr_dir));
	}
	/*
	else {
	if (!strcmp(type, "BLOB"))
	error = GIT_ENOTFOUND;
	else
	error = GIT_OK;
	}
	*/

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->tree_build) != 0)
		return GIT_ERROR;

	return GIT_OK;
}

git_tree *git_mysql_tree_root(git_mysql *mysql, git_repository *repo){

	int error;
	git_oid root;
	git_tree *tree;
	MYSQL_BIND bind_buffers[1];
	MYSQL_BIND result_buffers[4];

	memset(bind_buffers, 0, sizeof(bind_buffers));
	memset(result_buffers, 0, sizeof(result_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &(mysql->repo);
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;


	if (mysql_stmt_bind_param(mysql->tree_root, bind_buffers) != 0)
		return NULL;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_root) != 0)
		return NULL;

	if (mysql_stmt_store_result(mysql->tree_root) != 0)
		return NULL;

	if (mysql_stmt_num_rows(mysql->tree_root) > 0){
		git_oid oid;
		char *dir;
		char *entry;
		char *type;
		unsigned long dir_len;
		unsigned long entry_len;
		unsigned long type_len;

		result_buffers[0].buffer_type = MYSQL_TYPE_BLOB;
		result_buffers[0].buffer = &oid;
		result_buffers[0].buffer_length = 20;
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;
		memset(&oid, 0, 20);

		result_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[1].buffer = 0;
		result_buffers[1].buffer_length = 0;
		result_buffers[1].is_null = 0;
		result_buffers[1].length = &dir_len;

		result_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[2].buffer = 0;
		result_buffers[2].buffer_length = 0;
		result_buffers[2].is_null = 0;
		result_buffers[2].length = &entry_len;

		result_buffers[3].buffer_type = MYSQL_TYPE_STRING;
		result_buffers[3].buffer = 0;
		result_buffers[3].buffer_length = 0;
		result_buffers[3].is_null = 0;
		result_buffers[3].length = &type_len;

		if (mysql_stmt_bind_result(mysql->tree_root, result_buffers) != 0)
			return NULL;

		git_treebuilder   *bld = NULL;
		git_treebuilder_new(&bld, repo, NULL);

		while (mysql_stmt_fetch(mysql->tree_root) == MYSQL_DATA_TRUNCATED){

			dir = malloc(dir_len + 1);
			result_buffers[1].buffer = dir;
			result_buffers[1].buffer_length = dir_len + 1;
			if (mysql_stmt_fetch_column(mysql->tree_root, &result_buffers[1], 1, 0) != 0)
				return NULL;

			entry = malloc(entry_len + 1);
			result_buffers[2].buffer = entry;
			result_buffers[2].buffer_length = entry_len + 1;
			if (mysql_stmt_fetch_column(mysql->tree_root, &result_buffers[2], 2, 0) != 0)
				return NULL;

			type = malloc(type_len + 1);
			result_buffers[3].buffer = type;
			result_buffers[3].buffer_length = type_len + 1;
			if (mysql_stmt_fetch_column(mysql->tree_root, &result_buffers[3], 3, 0) != 0)
				return NULL;

			if (!strcmp(type, "BLOB"))
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_BLOB);
			else
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_TREE);

			free(dir);
			free(entry);
			free(type);
		};


		error = git_treebuilder_write(&root, bld);
		git_mysql_tree_update(mysql, "/", &root);
		git_treebuilder_free(bld);

		error = git_tree_lookup(&tree, repo, &root);
	}
	else
		error = GIT_ENOTFOUND;


	// reset the statement for further use
	if (mysql_stmt_reset(mysql->tree_root) != 0)
		return NULL;

	if (!error)
		return tree;
	else
		return NULL;
}


int tree_walk_cb(const char *root, const git_tree_entry *entry, void *payload)
{
	const char *name = git_tree_entry_name(entry);

	char sha1[GIT_OID_HEXSZ + 1];
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, git_tree_entry_id(entry));
	
	printf("entry: %s \n", name);
	printf("oid: %s \n", sha1);

    return 0;
}

int git_mysql_tree_walk(git_mysql *mysql, git_repository *repo){
	int error;
	git_oid oid;
	git_tree *tree = NULL;

	git_oid_fromstr(&oid, "352991f53e11f93fb6c6d729b8251d5b762547a8");

	//git_oid_fromstr(&oid, "7969f8b194a15e5ae6fec35b65c26c21c49fee23");

	error = git_tree_lookup(&tree, repo, &oid);

	error = git_tree_walk(tree, GIT_TREEWALK_PRE, tree_walk_cb, NULL);

	return error;
}


int each_file_cb(const git_diff_delta *delta,float progress,void *payload)
{
	printf("new file:%s \n", delta->new_file.path);
	printf("old file:%s \n", delta->old_file.path);

	return 0;
}

int each_hunk_cb(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	void *payload)
{

	//printf("hunk header:%s \n", hunk->header);

	return 0;
}

int each_line_cb(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	void *payload)
{
	//printf("line content:%s \n", line->content);

	return 0;
}

int git_mysql_tree_diff(git_diff *diff){
	int error;

	error = git_diff_foreach(diff,
		each_file_cb,
		each_hunk_cb,
		each_line_cb,
		NULL);

	return error;
}