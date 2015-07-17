#ifndef INCLUDE_git_mysql_repo_h__
#define INCLUDE_git_mysql_repo_h__

#include "git2.h"
#include "mysql_backend.h"

my_ulonglong git_mysql_repo_create(git_mysql *mysql, const char *name, const char *description);

int git_mysql_repo_del(git_mysql *mysql);

#endif
