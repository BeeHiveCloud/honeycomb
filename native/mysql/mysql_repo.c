#include "mysql_repo.h"

#ifdef GIT_WIN32
#include <winsock.h>
#endif

#include <mysql.h>

int git_mysql_repo_create(git_mysql *mysql, const long long int owner, const char *name, const char *description){

	MYSQL_BIND bind_buffers[3];
	my_ulonglong affected_rows;
	long long int repo;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = &owner;
	bind_buffers[0].buffer_length = sizeof(owner);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

	bind_buffers[1].buffer = name;
	bind_buffers[1].buffer_length = strlen(name);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	bind_buffers[2].buffer = description;
	bind_buffers[2].buffer_length = strlen(description);
	bind_buffers[2].length = &bind_buffers[2].buffer_length;
	bind_buffers[2].buffer_type = MYSQL_TYPE_BLOB;

	if (mysql_stmt_bind_param(mysql->repo_create, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_create) != 0)
		return GIT_ERROR;

	// now lets see if the insert worked
	affected_rows = mysql_stmt_affected_rows(mysql->repo_create);
	if (affected_rows != 1)
		return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(mysql->repo_create) != 0)
		return GIT_ERROR;

	return GIT_OK;
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

long long int git_mysql_last_seq(git_mysql *mysql){

	MYSQL_BIND result_buffers[1];
	long long int repo;

	// execute the statement
	if (mysql_stmt_execute(mysql->last_seq) != 0)
		return -1;

	if (mysql_stmt_store_result(mysql->last_seq) != 0)
		return -1;

	if (mysql_stmt_num_rows(mysql->last_seq) == 1){
		memset(result_buffers, 0, sizeof(result_buffers));

		result_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;
		result_buffers[0].buffer = &repo;
		result_buffers[0].buffer_length = sizeof(repo);
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;
		memset(&repo, 0, sizeof(repo));

		if (mysql_stmt_bind_result(mysql->last_seq, result_buffers) != 0)
			return -1;

		mysql_stmt_fetch(mysql->last_seq);
		
		return repo;
	}
	else
		return -1;


	// reset the statement for further use
	if (mysql_stmt_reset(mysql->last_seq) != 0)
		return -1;

	return -1;
}
