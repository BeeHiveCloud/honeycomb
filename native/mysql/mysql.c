#include "mysql.h"


void mysql_trx_start(MYSQL *db){
	mysql_query(db, "START TRANSACTION");
}

void mysql_trx_commit(MYSQL *db){
	mysql_query(db, "COMMIT");
}

void mysql_trx_rollback(MYSQL *db){
	mysql_query(db, "ROLLBACK");
}

int mysql_set_variable(const char *name, const char *expr){
  return 0;
}

const char * mysql_get_variable(const char *name){
  return NULL;
}
