#ifndef INCLUDE_git_mysql_repo_h__
#define INCLUDE_git_mysql_repo_h__

#include "git2.h"

GIT_BEGIN_DECL

GIT_EXTERN(long) git_repo_create(const long owner, const char *name, const char *description);

GIT_END_DECL

#endif
