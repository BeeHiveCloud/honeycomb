#ifndef INCLUDE_git_mysql_refdb_h__
#define INCLUDE_git_mysql_refdb_h__

#include <assert.h>
#include <string.h>

#include "git2.h"
#include "git2/sys/refdb_backend.h"
#include "git2/sys/refs.h"

#include "mysql_backend.h"

typedef struct {
	git_reference_iterator parent;
	git_mysql *mysql;
}git_mysql_ref_iterator;

typedef struct {
	git_refdb_backend parent;
	git_mysql *mysql;
} git_mysql_refdb;

GIT_BEGIN_DECL

GIT_EXTERN(int) git_mysql_refdb_init(git_refdb_backend **out, git_mysql *mysql);

GIT_END_DECL

#endif
