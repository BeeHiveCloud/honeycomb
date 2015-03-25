#ifndef INCLUDE_git_mysql_backend_h__
#define INCLUDE_git_mysql_backend_h__

#include <assert.h>
#include <string.h>

#include "git2.h"

#ifdef GIT_WIN32
#include <winsock.h>
#endif

#include <mysql.h>

typedef struct {
  MYSQL *db;
  MYSQL_STMT *odb_read;
  MYSQL_STMT *odb_write;
  MYSQL_STMT *odb_read_header;
  MYSQL_STMT *index_read;
  MYSQL_STMT *index_write;
} git_mysql;

GIT_BEGIN_DECL

GIT_EXTERN(int) git_mysql_init(git_mysql **out,
							   const char *mysql_host,
          					   const char *mysql_user,
          					   const char *mysql_passwd,
          					   const char *mysql_db,
          					   unsigned int mysql_port,
							   const char *mysql_unix_socket,
							   unsigned long mysql_client_flag);

GIT_EXTERN(int) git_mysql_free(git_mysql *mysql);

GIT_EXTERN(void) git_mysql_transaction(git_mysql *mysql);
GIT_EXTERN(void) git_mysql_commit(git_mysql *mysql);
GIT_EXTERN(void) git_mysql_rollback(git_mysql *mysql);

GIT_END_DECL

#endif
