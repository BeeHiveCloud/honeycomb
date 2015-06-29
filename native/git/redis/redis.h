#ifndef INCLUDE_git_redis_h__
#define INCLUDE_git_redis_h__

#include <assert.h>
#include <string.h>
#include <git2.h>
#include <git2/sys/odb_backend.h>
#include <git2/sys/refdb_backend.h>
#include <git2/sys/refs.h>
#include <hiredis.h>

typedef struct {
	git_odb_backend parent;

	char *prefix;
	char *repo_path;
	redisContext *db;
} hiredis_odb_backend;

typedef struct {
	git_refdb_backend parent;

	char *prefix;
	char *repo_path;
	redisContext *db;
} hiredis_refdb_backend;

typedef struct {
	git_reference_iterator parent;

	size_t current;
	redisReply *keys;

	hiredis_refdb_backend *backend;
} hiredis_refdb_iterator;

static redisContext *sharedConnection = NULL;

int git_refdb_backend_hiredis(git_refdb_backend **backend_out,
  const char* prefix, const char* path, const char *host, int port, char* password);

int git_odb_backend_hiredis(git_odb_backend **backend_out,
   const char* prefix, const char* path, const char *host, int port, char* password);

#endif
