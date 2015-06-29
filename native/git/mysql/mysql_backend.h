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
  my_ulonglong repo;
  MYSQL_STMT *last_seq;
  MYSQL_STMT *odb_read;
  MYSQL_STMT *odb_write;
  MYSQL_STMT *odb_read_header;
  MYSQL_STMT *index_read;
  MYSQL_STMT *index_write;
  MYSQL_STMT *index_del;
  MYSQL_STMT *refdb_read;
  MYSQL_STMT *refdb_write;
  MYSQL_STMT *refdb_read_header;
  MYSQL_STMT *refdb_del;
  MYSQL_STMT *refdb_rename;
  MYSQL_STMT *repo_create;
  MYSQL_STMT *repo_del;
  MYSQL_STMT *tree_init;
  MYSQL_STMT *tree_update;
  MYSQL_STMT *tree_build;
  MYSQL_STMT *tree_root;
  MYSQL_STMT *config_get;
  MYSQL_STMT *config_set;
  MYSQL_STMT *config_del;
} git_mysql;

int git_mysql_init(git_mysql **out,const char *mysql_host, const char *mysql_user,const char *mysql_passwd,const char *mysql_db,unsigned int mysql_port,const char *mysql_unix_socket,unsigned long mysql_client_flag);
int git_mysql_free(git_mysql *mysql);
void git_mysql_transaction(git_mysql *mysql);
void git_mysql_commit(git_mysql *mysql);
void git_mysql_rollback(git_mysql *mysql);
int git_mysql_adhoc(git_mysql *mysql, const char *cmd);

#endif
