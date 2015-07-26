#include "mysql_tree.h"

#include <stdio.h>

int git_mysql_tree_init(git_mysql *mysql){

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_init))
		return GIT_ERROR;

	return GIT_OK;
}

int git_mysql_tree_update(git_mysql *mysql, const char *dir, git_oid *oid){

	MYSQL_BIND bind_buffers[2];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the dir passed to the statement
	bind_buffers[0].buffer = (void *)dir;
	bind_buffers[0].buffer_length = strlen(dir) ;
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	// bind the oid passed to the statement
	bind_buffers[1].buffer = oid;
	bind_buffers[1].buffer_length = 20;
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

	if (mysql_stmt_bind_param(mysql->tree_update, bind_buffers))
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_update))
		return GIT_ERROR;

	return GIT_OK;
}


int git_mysql_tree_build(git_mysql *mysql, git_repository *repo, const char *type){

	int error;
	MYSQL_BIND bind_buffers[1];
	MYSQL_BIND result_buffers[3];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind type
	bind_buffers[0].buffer = (void *)type;
	bind_buffers[0].buffer_length = strlen(type);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_STRING;

	if (mysql_stmt_bind_param(mysql->tree_build, bind_buffers))
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_build))
		return GIT_ERROR;

	if (mysql_stmt_store_result(mysql->tree_build))
		return GIT_ERROR;

	if (mysql_stmt_num_rows(mysql->tree_build) > 0){

		memset(result_buffers, 0, sizeof(result_buffers));

		git_oid oid;
		char *dir;
		char *entry;

		result_buffers[0].buffer_type = MYSQL_TYPE_BLOB;
		result_buffers[0].buffer = &oid;
		result_buffers[0].buffer_length = 20;
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;
		memset(&oid, 0, 20);
        
        dir = calloc(1, mysql->meta_tree_build->fields[1].max_length + 1);
		result_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[1].buffer = dir;
		result_buffers[1].buffer_length = mysql->meta_tree_build->fields[1].max_length + 1;
		result_buffers[1].is_null = 0;
		result_buffers[1].length = &result_buffers[1].buffer_length;
        
        entry = calloc(1, mysql->meta_tree_build->fields[2].max_length + 1);
		result_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[2].buffer = 0;
		result_buffers[2].buffer_length = mysql->meta_tree_build->fields[2].max_length + 1;
		result_buffers[2].is_null = 0;
		result_buffers[2].length = &result_buffers[2].buffer_length;

		if (mysql_stmt_bind_result(mysql->tree_build, result_buffers))
			return GIT_ERROR;

		git_oid tree;
		git_treebuilder   *bld = NULL;
		git_treebuilder_new(&bld, repo, NULL);

		char curr_dir[1024];
        memset(curr_dir, 0, sizeof(curr_dir));

		while (!mysql_stmt_fetch(mysql->tree_build)){

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
		
	}

	return GIT_OK;
}

git_tree *git_mysql_tree_root(git_mysql *mysql, git_repository *repo){

	int error;
	git_oid root;
	git_tree *tree = NULL;

	MYSQL_BIND result_buffers[4];

	// execute the statement
	if (mysql_stmt_execute(mysql->tree_root))
		return NULL;

	if (mysql_stmt_store_result(mysql->tree_root))
		return NULL;

	if (mysql_stmt_num_rows(mysql->tree_root) > 0){
		git_oid oid;
		char *dir;
		char *entry;
		char type[5];

        memset(result_buffers, 0, sizeof(result_buffers));

		result_buffers[0].buffer_type = MYSQL_TYPE_BLOB;
		result_buffers[0].buffer = &oid;
		result_buffers[0].buffer_length = 20;
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;
		memset(&oid, 0, 20);
        
        dir = calloc(1, mysql->meta_tree_root->fields[1].max_length + 1);
		result_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;
        result_buffers[1].buffer = dir;
		result_buffers[1].buffer_length = mysql->meta_tree_root->fields[1].max_length + 1;
		result_buffers[1].is_null = 0;
		result_buffers[1].length = &result_buffers[1].buffer_length;
        
        entry = calloc(1, mysql->meta_tree_root->fields[2].max_length + 1);
		result_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[2].buffer = entry;
		result_buffers[2].buffer_length = mysql->meta_tree_root->fields[2].max_length + 1;
		result_buffers[2].is_null = 0;
		result_buffers[2].length = &result_buffers[2].buffer_length;
        
        memset(type, 0, sizeof(type));
		result_buffers[3].buffer_type = MYSQL_TYPE_STRING;
		result_buffers[3].buffer = type;
		result_buffers[3].buffer_length = 5;
		result_buffers[3].is_null = 0;
		result_buffers[3].length = &result_buffers[3].buffer_length;

		if (mysql_stmt_bind_result(mysql->tree_root, result_buffers))
			return NULL;

		git_treebuilder   *bld = NULL;
		git_treebuilder_new(&bld, repo, NULL);

		while (!mysql_stmt_fetch(mysql->tree_root)){

			if (!strcmp(type, "BLOB"))
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_BLOB);
			else
				error = git_treebuilder_insert(NULL, bld, entry, &oid, GIT_FILEMODE_TREE);

			free(dir);
			free(entry);
		};


		error = git_treebuilder_write(&root, bld);
		git_mysql_tree_update(mysql, "/", &root);
		git_treebuilder_free(bld);

		error = git_tree_lookup(&tree, repo, &root);
		if(error < 0){
			printf("git_mysql_tree_root git_tree_lookup\n");
			return NULL;
		}
	}
	else
		error = GIT_ENOTFOUND;

	if (!error)
		return tree;
	else
		return NULL;
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

	printf("hunk header:%s \n", hunk->header);

	return 0;
}

int each_line_cb(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	void *payload)
{
	printf("line content:%s \n", line->content);

	return 0;
}

int each_binary_cb(const git_diff_delta *delta, const git_diff_binary *binary, void *payload){

	return 0;
}

int git_mysql_tree_diff(git_diff *diff){
	int error;

	error = git_diff_foreach(diff,
		each_file_cb,
		NULL,
		NULL,
		NULL,
		NULL);

	return error;
}
