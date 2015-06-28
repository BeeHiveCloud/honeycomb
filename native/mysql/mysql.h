#ifndef INCLUDE_MYSQL_H_
#define INCLUDE_MYSQL_H_

#ifdef WIN32
#include <winsock.h>
#endif

#include <mysql.h>

#include "../util/util.h"

HC_BEGIN_DECL

HC_EXTERN(void) mysql_trx_start(MYSQL *db);

HC_EXTERN(void) mysql_trx_commit(MYSQL *db);

HC_EXTERN(void) mysql_trx_rollback(MYSQL *db);

HC_EXTERN(int) mysql_set_variable(const char *name, const char *expr);

HC_EXTERN(const char *) mysql_get_variable(const char *name);

HC_END_DECL

#endif
