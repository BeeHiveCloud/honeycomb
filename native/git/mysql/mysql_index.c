#include "mysql_index.h"

int git_mysql_index_write(git_mysql *mysql, git_oid *oid, const char *path){

	MYSQL_BIND bind_buffers[2];

	assert(mysql && path && oid);

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the oid
	bind_buffers[0].buffer = (void*)oid->id;
	bind_buffers[0].buffer_length = 20;
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

	// bind the name
	bind_buffers[1].buffer = (void*)path;
	bind_buffers[1].buffer_length = strlen(path);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(mysql->index_write, bind_buffers))
		return GIT_ERROR;

	if (mysql_stmt_execute(mysql->index_write))
		return GIT_ERROR;

	return GIT_OK;
}

int git_mysql_index_del(git_mysql *mysql, const char *path){

	MYSQL_BIND bind_buffers[1];

	assert(mysql && path);

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the path
	bind_buffers[0].buffer = (void*)path;
	bind_buffers[0].buffer_length = strlen(path);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(mysql->index_del, bind_buffers))
		return GIT_ERROR;

	if (mysql_stmt_execute(mysql->index_del))
		return GIT_ERROR;

	return GIT_OK;
}
