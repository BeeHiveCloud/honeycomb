#include "mysql_index.h"

int git_mysql_index_write(git_mysql *mysql, git_oid *oid, const char *path){
	//int error;
	MYSQL_BIND bind_buffers[3];
	my_ulonglong affected_rows;

	assert(mysql && path && oid);

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &mysql->repo;
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	// bind the oid
	bind_buffers[1].buffer = (void*)oid->id;
	bind_buffers[1].buffer_length = 20;
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

	// bind the name
	bind_buffers[2].buffer = (void*)path;
	bind_buffers[2].buffer_length = strlen(path);
	bind_buffers[2].length = &bind_buffers[2].buffer_length;
	bind_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(mysql->index_write, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->index_write) != 0)
		return GIT_ERROR;

	// now lets see if the insert worked
	affected_rows = mysql_stmt_affected_rows(mysql->index_write);
	//if (affected_rows > 1)
	//	return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->index_write) != 0)
		return GIT_ERROR;

	return GIT_OK;
}

int git_mysql_index_del(git_mysql *mysql, const char *path){
	//int error;
	MYSQL_BIND bind_buffers[2];
	my_ulonglong affected_rows;

	assert(mysql && path);

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &mysql->repo;
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	// bind the path
	bind_buffers[1].buffer = (void*)path;
	bind_buffers[1].buffer_length = strlen(path);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_STRING;

	if (mysql_stmt_bind_param(mysql->index_del, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->index_del) != 0)
		return GIT_ERROR;

	affected_rows = mysql_stmt_affected_rows(mysql->index_del);
	if (affected_rows != 1)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->index_del) != 0)
		return GIT_ERROR;

	return GIT_OK;
}
