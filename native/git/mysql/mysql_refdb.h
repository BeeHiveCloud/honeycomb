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

int git_mysql_refdb_init(git_refdb_backend **out, git_mysql *mysql);
int mysql_ref_iterator_next(git_reference **ref, git_reference_iterator *iter);
int mysql_ref_iterator_next_name(const char **ref_name, git_reference_iterator *iter);
int mysql_ref_iterator_free(git_reference_iterator *iter);
int mysql_refdb_exists(int *exists, git_refdb_backend *_backend, const char *ref_name);
int mysql_refdb_lookup(git_reference **out, git_refdb_backend *_backend, const char *ref_name);
int mysql_refdb_iterator(git_reference_iterator **iter, git_refdb_backend *backend, const char *glob);
int mysql_refdb_write(git_refdb_backend *_backend,
                      const git_reference *ref, int force,
                      const git_signature *who, const char *message,
                      const git_oid *old, const char *old_target);
int mysql_refdb_rename(git_reference **out, git_refdb_backend *backend,
                       const char *old_name, const char *new_name, int force,
                       const git_signature *who, const char *message);
void mysql_refdb_free(git_refdb_backend *_backend);

#endif
