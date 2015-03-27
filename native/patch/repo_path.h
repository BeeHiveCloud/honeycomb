#ifndef LIBGIT2_PATCH_REPO_H
#define LIBGIT2_PATCH_REPO_H

#include "git2.h"

GIT_BEGIN_DECL

GIT_EXTERN(int) git_repository_set_path(git_repository *repo, const char *path);

GIT_END_DECL

#endif
