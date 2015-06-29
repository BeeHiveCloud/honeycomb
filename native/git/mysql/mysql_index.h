#ifndef INCLUDE_git_mysql_index_h__
#define INCLUDE_git_mysql_index_h__

#include "git2.h"
#include "mysql_backend.h"

int git_mysql_index_write(git_mysql *mysql, git_oid *oid, const char *path);

int git_mysql_index_del(git_mysql *mysql, const char *path);

#endif
