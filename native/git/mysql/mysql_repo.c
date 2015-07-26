#include "mysql_repo.h"

#ifdef GIT_WIN32
#include <winsock.h>
#endif

#include <mysql.h>

#include <stdio.h>

my_ulonglong git_mysql_repo_create(git_mysql *mysql, const char *name, const char *description){

	MYSQL_BIND bind_buffers[2];

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the repo passed to the statement
	bind_buffers[0].buffer = (void *)name;
	bind_buffers[0].buffer_length = strlen(name);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	bind_buffers[1].buffer = (void *)description;
	bind_buffers[1].buffer_length = strlen(description);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(mysql->repo_create, bind_buffers))
		return 0;

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_create))
		return 0;

	return mysql_stmt_insert_id(mysql->repo_create);

}

int git_mysql_repo_del(git_mysql *mysql){

	// execute the statement
	if (mysql_stmt_execute(mysql->repo_del))
		return GIT_ERROR;

	return GIT_OK;
}

int git_mysql_repo_exists(git_mysql *mysql, const char* name){
    
    MYSQL_BIND bind_buffers[1];
    int exists = 0;
    
    memset(bind_buffers, 0, sizeof(bind_buffers));
    
    // bind the repo passed to the statement
    bind_buffers[0].buffer = (void *)name;
    bind_buffers[0].buffer_length = strlen(name);
    bind_buffers[0].length = &bind_buffers[0].buffer_length;
    bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    
    if (mysql_stmt_bind_param(mysql->repo_exists, bind_buffers))
        return GIT_ERROR;
    
    // execute the statement
    if (mysql_stmt_execute(mysql->repo_exists))
        return GIT_ERROR;
    
    if (mysql_stmt_store_result(mysql->repo_exists))
        return GIT_ERROR;
    
    // this should either be 0 or 1
    if (mysql_stmt_num_rows(mysql->repo_exists) == 1)
        exists = 1;
    
    return exists;
}
