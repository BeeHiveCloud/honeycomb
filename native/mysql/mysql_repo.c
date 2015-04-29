#include "mysql_repo.h"

#ifdef GIT_WIN32
#include <winsock.h>
#endif

#include <mysql.h>

char *git_mysql_repo_create(git_mysql *mysql, const long long int owner, const char *name, const char *description){

	MYSQL_BIND bind_buffers[3];
	my_ulonglong affected_rows;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &owner;
	bind_buffers[0].buffer_length = sizeof(owner);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	bind_buffers[1].buffer = name;
	bind_buffers[1].buffer_length = strlen(name)+1;
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	bind_buffers[2].buffer = description;
	bind_buffers[2].buffer_length = strlen(description) + 1;
	bind_buffers[2].length = &bind_buffers[2].buffer_length;
	bind_buffers[2].buffer_type = MYSQL_TYPE_BLOB;

	if (mysql_stmt_bind_param(mysql->repo_create, bind_buffers) != 0)
		return NULL;

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_create) != 0)
		return NULL;

	// now lets see if the insert worked
	affected_rows = mysql_stmt_affected_rows(mysql->repo_create);
	if (affected_rows != 1)
		return NULL;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->repo_create) != 0)
		return NULL;

	mysql_query(mysql->db, "SELECT LAST_INSERT_ID();");

	MYSQL_RES *result = mysql_store_result(mysql->db);

	MYSQL_ROW row = mysql_fetch_row(result);

	mysql_free_result(result);

	return row[0];
}

int git_mysql_repo_del(git_mysql *mysql){

	MYSQL_BIND bind_buffers[1];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &(mysql->repo);
	bind_buffers[0].buffer_length = sizeof(mysql->repo);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	if (mysql_stmt_bind_param(mysql->repo_del, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_del) != 0)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->repo_del) != 0)
		return GIT_ERROR;

	return GIT_OK;
}