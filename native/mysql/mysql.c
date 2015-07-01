#include "mysql.h"

#include <stdio.h>

MYSQL *mysql_hc_connect(const char *mysql_host, const char *mysql_user, const char *mysql_passwd, const char *mysql_db, unsigned int mysql_port, const char *mysql_unix_socket, unsigned long mysql_client_flag){
    
  MYSQL *db = NULL;
  my_bool reconnect = 1;

	if (mysql_library_init(0, NULL, NULL))
		return NULL;

	db = mysql_init(db);

	if (mysql_options(db, MYSQL_OPT_RECONNECT, &reconnect) != 0){
		mysql_hc_disconnect(db);
		return NULL;
	}

	if (mysql_real_connect(db, mysql_host, mysql_user, mysql_passwd, mysql_db, mysql_port, mysql_unix_socket,mysql_client_flag) != db){
		mysql_hc_disconnect(db);
		return NULL;
	}

	mysql_autocommit(db, 0);

	return db;
}

void mysql_hc_disconnect(MYSQL *db){
	mysql_close(db);
	mysql_library_end();
}

void mysql_trx_start(MYSQL *db){
	mysql_query(db, "START TRANSACTION");
}

void mysql_trx_commit(MYSQL *db){
	mysql_query(db, "COMMIT");
}

void mysql_trx_rollback(MYSQL *db){
	mysql_query(db, "ROLLBACK");
}

int mysql_hc_set_variable(MYSQL *db, const char *name, const char *value){
    
    char *sql = malloc(strlen(name)+strlen(value)+8);
    
    sprintf(sql,"SET @%s=%s;",name,value);
    
    if(mysql_query(db, sql))
        return -1;
    
    free((void *)sql);
    
    return 0;
}

const char * mysql_hc_get_variable(MYSQL *db, const char *name){

    char *sql = malloc(strlen(name)+10);
 
    sprintf(sql,"SELECT @%s;",name);
    
    if(mysql_query(db, sql))
        return -1;
    
    MYSQL_RES *result = mysql_store_result(db);
    
    MYSQL_ROW row = mysql_fetch_row(result);
    
    mysql_free_result(result);
    
    free((void *)sql);
    
    return row[0];
}

