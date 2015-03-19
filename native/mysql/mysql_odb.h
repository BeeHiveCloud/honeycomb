#ifndef INCLUDE_git_mysql_odb_h__
#define INCLUDE_git_mysql_odb_h__

#include <assert.h>
#include <string.h>

#include "git2.h"
#include "git2/sys/odb_backend.h"

#include "mysql_backend.h"

typedef struct {
	git_odb_backend parent;
	git_mysql *mysql;
} git_mysql_odb;

GIT_BEGIN_DECL

GIT_EXTERN(int) git_mysql_odb_init(git_odb_backend **out, git_mysql *mysql);

GIT_END_DECL

#endif
