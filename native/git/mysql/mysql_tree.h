#ifndef INCLUDE_git_mysql_tree_h__
#define INCLUDE_git_mysql_tree_h__

#include "git2.h"
#include "mysql_backend.h"

int git_mysql_tree_init(git_mysql *mysql);

int git_mysql_tree_build(git_mysql *mysql, git_repository *repo, const char *type);

git_tree *git_mysql_tree_root(git_mysql *mysql, git_repository *repo);

int git_mysql_tree_walk(git_mysql *mysql, git_repository *repo);

int git_mysql_tree_diff(git_diff *diff);

#endif
