#ifndef INCLUDE_git_mysql_repo_h__
#define INCLUDE_git_mysql_repo_h__

#include "git2.h"
//#include "../../vendor/libgit2/src/repository.h"

GIT_BEGIN_DECL

GIT_EXTERN(int) git_repo_set_dummy_path(git_repository *repo);

GIT_END_DECL

#endif
