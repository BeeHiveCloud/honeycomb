#include "mysql_repo.h"

#ifdef GIT_WIN32
#include <winsock.h>
#endif

#include <mysql.h>

my_ulonglong git_mysql_repo_create(git_mysql *mysql, const char *name, const char *description){

	MYSQL_BIND bind_buffers[2];
	my_ulonglong affected_rows = 0;
	my_ulonglong rid = 0;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = (void *)name;
	bind_buffers[0].buffer_length = strlen(name);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	bind_buffers[1].buffer = (void *)description;
	bind_buffers[1].buffer_length = strlen(description);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

	if (mysql_stmt_bind_param(mysql->repo_create, bind_buffers) != 0)
		return 0;

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_create) != 0)
		return 0;

	// now lets see if the insert worked
	affected_rows = mysql_stmt_affected_rows(mysql->repo_create);
	if (affected_rows != 1)
		return 0;

	rid = mysql_stmt_insert_id(mysql->repo_create);

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->repo_create) != 0)
		return 0;

	return rid;
}

int git_mysql_repo_del(git_mysql *mysql){

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_del) != 0)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->repo_del) != 0)
		return GIT_ERROR;

	return GIT_OK;
}
