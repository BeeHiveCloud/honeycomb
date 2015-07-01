#include <stdio.h>

#include "../mysql/mysql.h"

int main(int argc, char **argv){
    MYSQL *db;
    db = mysql_hc_connect("localhost","git","git","git",0,NULL,0);
    mysql_hc_set_variable(db, "hc_v1","123");
    printf("%s\n",mysql_hc_get_variable(db, "hc_v1"));
    mysql_hc_disconnect(db);
    return 0;
}
