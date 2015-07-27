#include "mysql_backend.h"

int init_statements(git_mysql *mysql)
{
  my_bool truth = 1;
  //unsigned long type = (unsigned long)CURSOR_TYPE_READ_ONLY;

  static const char *sql_odb_read =
    "SELECT `type`, `size`, UNCOMPRESS(`data`) FROM GIT_ODB WHERE `repo` = @repo AND `oid` = ?;";

  static const char *sql_odb_read_header =
    "SELECT `type`, `size` FROM GIT_ODB WHERE `repo` = @repo AND `oid` = ?;";

  static const char *sql_odb_write =
    "INSERT INTO GIT_ODB VALUES (@repo, ?, ?, ?, COMPRESS(?));";

  static const char *sql_index_read =
	  "SELECT `oid`, `level`, `dir`, `file` FROM GIT_INDEX_V WHERE `repo` = @repo ORDER BY `level` DESC, `dir`;";

  static const char *sql_index_write =
	"INSERT INTO GIT_INDEX VALUES (@repo, ?, ?) ON DUPLICATE KEY UPDATE `oid` = VALUES(`oid`);";

  static const char *sql_index_del =
	  "DELETE FROM GIT_INDEX WHERE `repo` = @repo AND `path` = ?;";

  static const char *sql_refdb_read =
	  "SELECT `type`, `target` FROM GIT_REFDB WHERE `repo` = CONVERT(@repo, UNSIGNED) AND `name` = ?;";

  static const char *sql_refdb_read_header =
	  "SELECT `type` FROM GIT_REFDB WHERE `repo` = @repo AND `name` = ?;";

  static const char *sql_refdb_write =
	  "INSERT IGNORE INTO GIT_REFDB VALUES (@repo, ?, ?, ?) ON DUPLICATE KEY UPDATE `type` = VALUES(`type`), `target` = VALUES(`target`);";

  static const char *sql_refdb_del =
	  "DELETE FROM GIT_REFDB WHERE `repo` = @repo AND `name` = ?;";

  static const char *sql_repo_create =
	  "INSERT INTO GIT_REPO(`owner`,`name`,`description`) VALUES (@user, ?, ?);";

  static const char *sql_repo_del = "CALL GIT_REPO_DEL(@repo);";

  static const char *sql_repo_exists =
    "SELECT `id` FROM GIT_REPO WHERE `owner` = @user AND `name` = ?;";
    
  static const char *sql_tree_init = "CALL GIT_TREE_INIT(@repo);";
  static const char *sql_tree_update = "CALL GIT_TREE_UPDATE(@repo, ?, ?);";

  static const char *sql_tree_build =
	  "SELECT `oid`, `dir`, `entry` FROM GIT_TREE WHERE `repo` = @repo AND `type` = ? AND `dir` <> '/' ORDER BY `dir`, `entry`;";

  static const char *sql_tree_root =
	  "SELECT `oid`, `dir`, `entry`, `type` FROM GIT_TREE WHERE `repo` = @repo AND `dir` = '/' ORDER BY `type` DESC, `entry`;";

  static const char *sql_config_get =
	  "SELECT `value` FROM GIT_CONFIG WHERE `repo` = @repo AND `key` = ?;";

  static const char *sql_config_set =
	  "INSERT INTO GIT_CONFIG VALUES(@repo, ?, ?) ON DUPLICATE KEY UPDATE `value` = VALUES(`value`);";

  static const char *sql_config_del =
	  "DELETE FROM GIT_CONFIG WHERE `repo` = @repo AND `key` = ?;";

  mysql->odb_read = mysql_stmt_init(mysql->db);
  if (mysql->odb_read == NULL)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_read, sql_odb_read, strlen(sql_odb_read)) != 0)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_read, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  mysql->meta_odb = mysql_stmt_result_metadata(mysql->odb_read);
    
  //if (mysql_stmt_attr_set(mysql->odb_read, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;


  mysql->odb_read_header = mysql_stmt_init(mysql->db);
  if (mysql->odb_read_header == NULL)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_read_header, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_read_header, sql_odb_read_header, strlen(sql_odb_read_header)) != 0)
    return GIT_ERROR;

  //if (mysql_stmt_attr_set(mysql->odb_read_header, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;



  mysql->odb_write = mysql_stmt_init(mysql->db);
  if (mysql->odb_write == NULL)
    return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->odb_write, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->odb_write, sql_odb_write, strlen(sql_odb_write)) != 0)
    return GIT_ERROR;


  mysql->index_read = mysql_stmt_init(mysql->db);
  if (mysql->index_read == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->index_read, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->index_read, sql_index_read, strlen(sql_index_read)) != 0)
	  return GIT_ERROR;

  //if (mysql_stmt_attr_set(mysql->index_read, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;


  mysql->index_write = mysql_stmt_init(mysql->db);
  if (mysql->index_write == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->index_write, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->index_write, sql_index_write, strlen(sql_index_write)) != 0)
	  return GIT_ERROR;


  mysql->index_del = mysql_stmt_init(mysql->db);
  if (mysql->index_del == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->index_del, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->index_del, sql_index_del, strlen(sql_index_del)) != 0)
	  return GIT_ERROR;


  mysql->refdb_read = mysql_stmt_init(mysql->db);
  if (mysql->refdb_read == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->refdb_read, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->refdb_read, sql_refdb_read, strlen(sql_refdb_read)) != 0)
	  return GIT_ERROR;

  mysql->meta_refdb = mysql_stmt_result_metadata(mysql->refdb_read);
    
  //if (mysql_stmt_attr_set(mysql->refdb_read, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;


  mysql->refdb_read_header = mysql_stmt_init(mysql->db);
  if (mysql->refdb_read_header == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->refdb_read_header, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->refdb_read_header, sql_refdb_read_header, strlen(sql_refdb_read_header)) != 0)
	  return GIT_ERROR;

  //if (mysql_stmt_attr_set(mysql->refdb_read_header, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;


  mysql->refdb_write = mysql_stmt_init(mysql->db);
  if (mysql->refdb_write == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->refdb_write, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->refdb_write, sql_refdb_write, strlen(sql_refdb_write)) != 0)
	  return GIT_ERROR;


  mysql->refdb_del = mysql_stmt_init(mysql->db);
  if (mysql->refdb_del == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->refdb_del, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->refdb_del, sql_refdb_del, strlen(sql_refdb_del)) != 0)
	  return GIT_ERROR;


  mysql->repo_create = mysql_stmt_init(mysql->db);
  if (mysql->repo_create == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->repo_create, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->repo_create, sql_repo_create, strlen(sql_repo_create)) != 0)
	  return GIT_ERROR;

  mysql->repo_del = mysql_stmt_init(mysql->db);
  if (mysql->repo_del == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->repo_del, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->repo_del, sql_repo_del, strlen(sql_repo_del)) != 0)
	  return GIT_ERROR;

  mysql->repo_exists = mysql_stmt_init(mysql->db);
  if (mysql->repo_exists == NULL)
      return GIT_ERROR;
    
  if (mysql_stmt_attr_set(mysql->repo_exists, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
      return GIT_ERROR;
    
  if (mysql_stmt_prepare(mysql->repo_exists, sql_repo_exists, strlen(sql_repo_exists)) != 0)
      return GIT_ERROR;
    
  mysql->tree_init = mysql_stmt_init(mysql->db);
  if (mysql->tree_init == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->tree_init, sql_tree_init, strlen(sql_tree_init)) != 0)
	  return GIT_ERROR;

  mysql->tree_update = mysql_stmt_init(mysql->db);
  if (mysql->tree_update == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->tree_update, sql_tree_update, strlen(sql_tree_update)) != 0)
	  return GIT_ERROR;

  mysql->tree_build = mysql_stmt_init(mysql->db);
  if (mysql->tree_build == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->tree_build, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->tree_build, sql_tree_build, strlen(sql_tree_build)) != 0)
	  return GIT_ERROR;

  mysql->meta_tree_build = mysql_stmt_result_metadata(mysql->tree_build);
  //if (mysql_stmt_attr_set(mysql->tree_build, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;

  mysql->tree_root = mysql_stmt_init(mysql->db);
  if (mysql->tree_root == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->tree_root, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->tree_root, sql_tree_root, strlen(sql_tree_root)) != 0)
	  return GIT_ERROR;

  mysql->meta_tree_root = mysql_stmt_result_metadata(mysql->tree_root);
  //if (mysql_stmt_attr_set(mysql->tree_root, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
	//  return GIT_ERROR;


  mysql->config_get = mysql_stmt_init(mysql->db);
  if (mysql->config_get == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->config_get, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->config_get, sql_config_get, strlen(sql_config_get)) != 0)
	  return GIT_ERROR;

  mysql->meta_config = mysql_stmt_result_metadata(mysql->config_get);
    
 // if (mysql_stmt_attr_set(mysql->config_get, STMT_ATTR_CURSOR_TYPE, (const void *)&type) != 0)
// return GIT_ERROR;

  mysql->config_set = mysql_stmt_init(mysql->db);
  if (mysql->config_set == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->config_set, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->config_set, sql_config_set, strlen(sql_config_set)) != 0)
	  return GIT_ERROR;

  mysql->config_del = mysql_stmt_init(mysql->db);
  if (mysql->config_del == NULL)
	  return GIT_ERROR;

  if (mysql_stmt_attr_set(mysql->config_del, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_prepare(mysql->config_del, sql_config_del, strlen(sql_config_del)) != 0)
	  return GIT_ERROR;


  return GIT_OK;
}

int git_mysql_free(git_mysql *mysql)
{
  assert(mysql);

  if (mysql->odb_read){
        mysql_stmt_close(mysql->odb_read);
        mysql_free_result(mysql->meta_odb);
  }
  if (mysql->odb_read_header)
    mysql_stmt_close(mysql->odb_read_header);
  if (mysql->odb_write)
    mysql_stmt_close(mysql->odb_write);
  if (mysql->index_read)
	  mysql_stmt_close(mysql->index_read);
  if (mysql->index_write)
	  mysql_stmt_close(mysql->index_write);
  if (mysql->index_del)
	  mysql_stmt_close(mysql->index_del);
  if (mysql->refdb_read){
        mysql_stmt_close(mysql->refdb_read);
        mysql_free_result(mysql->meta_refdb);
  }
  if (mysql->refdb_read_header)
	  mysql_stmt_close(mysql->refdb_read_header);
  if (mysql->refdb_write)
	  mysql_stmt_close(mysql->refdb_write);
  if (mysql->refdb_del)
	  mysql_stmt_close(mysql->refdb_del);
  if (mysql->refdb_rename)
	  mysql_stmt_close(mysql->refdb_rename);
  if (mysql->repo_create)
	  mysql_stmt_close(mysql->repo_create);
  if (mysql->repo_del)
      mysql_stmt_close(mysql->repo_del);
  if (mysql->repo_exists)
      mysql_stmt_close(mysql->repo_exists);
  if (mysql->tree_init)
	  mysql_stmt_close(mysql->tree_init);
  if (mysql->tree_update)
	  mysql_stmt_close(mysql->tree_update);
  if (mysql->tree_build){
	  mysql_stmt_close(mysql->tree_build);
      mysql_free_result(mysql->meta_tree_build);
  }
  if (mysql->tree_root){
	  mysql_stmt_close(mysql->tree_root);
      mysql_free_result(mysql->meta_tree_root);
  }
  if (mysql->config_get){
	  mysql_stmt_close(mysql->config_get);
      mysql_free_result(mysql->meta_config);
  }
  if (mysql->config_set)
	  mysql_stmt_close(mysql->config_set);
  if (mysql->config_del)
	  mysql_stmt_close(mysql->config_del);

  free(mysql);

  return GIT_OK;
}

int git_mysql_init(git_mysql **out,MYSQL *db)
{
  git_mysql *mysql;
  int error = GIT_ERROR;

  mysql = calloc(1, sizeof(git_mysql));

  if (mysql == NULL)
    return GIT_ERROR;
    
  mysql->db = db;
    
  // init sql statements
  error = init_statements(mysql);
  if (error < 0)
	  return GIT_ERROR;

  *out = mysql;

  return GIT_OK;
}

