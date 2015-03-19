#include "mysql_backend.h"

int init_statements(git_mysql *mysql)
{
  my_bool truth = 1;

  static const char *sql_odb_read =
    "SELECT `type`, `size`, UNCOMPRESS(`data`) FROM GIT2_ODB WHERE `oid` = ?;";

  static const char *sql_odb_read_header =
    "SELECT `type`, `size` FROM GIT2_ODB WHERE `oid` = ?;";

  static const char *sql_odb_write =
    "INSERT IGNORE INTO GIT2_ODB VALUES (?, ?, ?, COMPRESS(?));";


  mysql->odb_read = mysql_stmt_init(mysql->db);
  if (mysql->odb_read == NULL)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_read, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_read, sql_odb_read, strlen(sql_odb_read)) != 0)
    return GIT_ERROR;


  mysql->odb_read_header = mysql_stmt_init(mysql->db);
  if (mysql->odb_read_header == NULL)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_read_header, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_read_header, sql_odb_read_header, strlen(sql_odb_read_header)) != 0)
    return GIT_ERROR;


  mysql->odb_write = mysql_stmt_init(mysql->db);
  if (mysql->odb_write == NULL)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_write, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_write, sql_odb_write, strlen(sql_odb_write)) != 0)
    return GIT_ERROR;


  return GIT_OK;
}

int git_mysql_free(git_mysql *mysql)
{
  assert(mysql);

  if (mysql->odb_read)
    mysql_stmt_close(mysql->odb_read);
  if (mysql->odb_read_header)
    mysql_stmt_close(mysql->odb_read_header);
  if (mysql->odb_write)
    mysql_stmt_close(mysql->odb_write);

  mysql_close(mysql->db);

  free(mysql);

  return GIT_OK;
}

int git_mysql_init(git_mysql **out, 
				   const char *mysql_host,
				   const char *mysql_user,
				   const char *mysql_passwd,
				   const char *mysql_db,
				   unsigned int mysql_port,
				   const char *mysql_unix_socket,
				   unsigned long mysql_client_flag)
{
  git_mysql *mysql;
  int error;
  my_bool reconnect;

  mysql = calloc(1, sizeof(git_mysql));

  if (mysql == NULL)
    return GIT_ERROR;

  mysql->db = mysql_init(mysql->db);

  reconnect = 1;

  // allow libmysql to reconnect gracefully
  if (mysql_options(mysql->db, MYSQL_OPT_RECONNECT, &reconnect) != 0){
    git_mysql_free(mysql);
    return GIT_ERROR;
  }

  // make the connection
  if (mysql_real_connect(mysql->db, mysql_host, mysql_user, mysql_passwd, mysql_db, mysql_port, mysql_unix_socket, mysql_client_flag) != mysql->db){
    git_mysql_free(mysql);
    return GIT_ERROR;
  }

  // init sql statements
  error = init_statements(mysql);
  if (error < 0)
	  return GIT_ERROR;

  *out = mysql;

  return GIT_OK;
}