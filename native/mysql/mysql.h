#ifndef INCLUDE_MYSQL_H_
#define INCLUDE_MYSQL_H_

#ifdef WIN32
#include <winsock.h>
#endif

#include <mysql.h>

void mysql_connect(const char *mysql_host, const char *mysql_user, const char *mysql_passwd, const char *mysql_db, unsigned int mysql_port, const char *mysql_unix_socket, unsigned long mysql_client_flag);

void mysql_disconnect(MYSQL *db);

void mysql_trx_start(MYSQL *db);

void mysql_trx_commit(MYSQL *db);

void mysql_trx_rollback(MYSQL *db);

int mysql_set_variable(const char *name, const char *expr);

const char *mysql_get_variable(const char *name);

#endif
