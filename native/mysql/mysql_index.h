#ifndef INCLUDE_git_mysql_index_h__
#define INCLUDE_git_mysql_index_h__

#include "git2.h"
#include "mysql_backend.h"


GIT_BEGIN_DECL

GIT_EXTERN(int) git_mysql_index_add(git_mysql *mysql, git_oid *oid, const char *path);

GIT_EXTERN(int) git_mysql_index_del(git_mysql *mysql, const char *path);

GIT_END_DECL

#endif
