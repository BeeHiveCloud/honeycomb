#include "mysql_index.h"

int git_mysql_index_write(git_mysql *mysql, git_oid *oid, const char *path){
	int error; 
	MYSQL_BIND bind_buffers[2];
	my_ulonglong affected_rows;

	assert(mysql && path && oid);

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the oid
	bind_buffers[0].buffer = (void*)oid->id;
	bind_buffers[0].buffer_length = 20;
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

	// bind the name
	bind_buffers[1].buffer = (void*)path;
	bind_buffers[1].buffer_length = strlen(path) + 1;
	//bind_buffers[1].length = &bind_buffers[0].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(mysql->index_write, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->index_write) != 0)
		return GIT_ERROR;

	// now lets see if the insert worked
	affected_rows = mysql_stmt_affected_rows(mysql->index_write);
	//if (affected_rows != 1)
	//	return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->index_write) != 0)
		return GIT_ERROR;
}

int git_mysql_index_read(git_oid *oid, git_mysql *mysql, const char *name){
	int error;
	MYSQL_BIND bind_buffers[1];
	MYSQL_BIND result_buffers[1];

	assert(mysql && name);

	memset(bind_buffers, 0, sizeof(bind_buffers));
	memset(result_buffers, 0, sizeof(result_buffers));

	// bind the name
	bind_buffers[0].buffer = (void*)name;
	bind_buffers[0].buffer_length = strlen(name) + 1;
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_STRING;

	if (mysql_stmt_bind_param(mysql->index_read, bind_buffers) != 0)
		return 0;

	// execute the statement
	if (mysql_stmt_execute(mysql->index_read) != 0)
		return 0;

	if (mysql_stmt_store_result(mysql->index_read) != 0)
		return 0;

	// this should either be 0 or 1
	if (mysql_stmt_num_rows(mysql->index_read) == 1){
		result_buffers[0].buffer_type = MYSQL_TYPE_BLOB;
		result_buffers[0].buffer = oid;
		result_buffers[0].buffer_length = 20;
		memset(oid, 0, 20);

		if (mysql_stmt_bind_result(mysql->index_read, result_buffers) != 0)
			return GIT_ERROR;

		// this should populate the buffers at *type_p and *len_p
		if (mysql_stmt_fetch(mysql->index_read) != 0)
			return GIT_ERROR;

		error = GIT_OK;
	}
	else {
		error = GIT_ENOTFOUND;
	}

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->index_read) != 0)
		return 0;

	return error;
}