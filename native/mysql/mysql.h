#ifndef INCLUDE_MYSQL_H_
#define INCLUDE_MYSQL_H_

#ifdef WIN32
#include <winsock.h>
#endif

#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

MYSQL *mysql_db_connect(const char *mysql_host, const char *mysql_user, const char *mysql_passwd, const char *mysql_db, unsigned int mysql_port, const char *mysql_unix_socket, unsigned long mysql_client_flag);

void mysql_db_disconnect(MYSQL *db);

void mysql_trx_start(MYSQL *db);

void mysql_trx_commit(MYSQL *db);

void mysql_trx_rollback(MYSQL *db);

int mysql_set_variable(MYSQL *db, const char *name, const char *value);

const char * mysql_get_variable(MYSQL *db, const char *name);

#endif
