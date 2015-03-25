#ifndef INCLUDE_git_mysql_repo_h__
#define INCLUDE_git_mysql_repo_h__

#include "git2.h"

GIT_BEGIN_DECL

GIT_EXTERN(int) git_repository_set_path(git_repository *repo, char *path);

GIT_END_DECL

#endif
