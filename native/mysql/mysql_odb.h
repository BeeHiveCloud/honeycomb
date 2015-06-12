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

int mysql_odb_read_prefix(git_oid *out_oid, void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *short_oid, size_t len);

int mysql_odb_read_header(size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid);

int mysql_odb_read(void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid);

int mysql_odb_exists(git_odb_backend *_backend, const git_oid *oid);

int mysql_odb_write(git_odb_backend *_backend, const git_oid *oid, const void *data, size_t len, git_otype type);

void mysql_odb_free(git_odb_backend *_backend);


GIT_BEGIN_DECL

GIT_EXTERN(int) git_mysql_odb_init(git_odb_backend **out, git_mysql *mysql);

GIT_END_DECL

#endif
