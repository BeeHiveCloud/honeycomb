#include "mysql_odb.h"

int mysql_odb_read_header(size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int error = GIT_ERROR;
  MYSQL_BIND bind_buffers[1];
  MYSQL_BIND result_buffers[2];
  MYSQL_RES  *prepare_meta_result = NULL;

  git_rawobj raw;

  assert(len_p && type_p && _backend && oid);

  memset(&raw, 0, sizeof(raw));

  backend = (git_mysql_odb *)_backend;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the oid passed to the statement
  bind_buffers[0].buffer = (void*)oid->id;
  bind_buffers[0].buffer_length = 20;
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

  if (mysql_stmt_bind_param(backend->mysql->odb_read_header, bind_buffers) != 0)
	  return GIT_ERROR;

  /* Fetch result set meta information */
  prepare_meta_result = mysql_stmt_result_metadata(backend->mysql->odb_read_header);
  if(!prepare_meta_result)
	return GIT_ERROR;

  // execute the statement
  if (mysql_stmt_execute(backend->mysql->odb_read_header) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_store_result(backend->mysql->odb_read_header) != 0)
	  return GIT_ERROR;

  // this should either be 0 or 1
  // if it's > 1 MySQL's unique index failed and we should all fear for our lives
  if (mysql_stmt_num_rows(backend->mysql->odb_read_header) == 1) {

	memset(result_buffers, 0, sizeof(result_buffers));

	result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
	result_buffers[0].buffer = (void *)&raw.type;
	result_buffers[0].buffer_length = sizeof(*type_p);
	result_buffers[0].is_null = 0;
	result_buffers[0].length = &result_buffers[0].buffer_length;

    result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
	result_buffers[1].buffer = (void *)&raw.len;
	result_buffers[1].buffer_length = sizeof(*len_p);
	result_buffers[1].is_null = 0;
	result_buffers[1].length = &result_buffers[1].buffer_length;

	if (mysql_stmt_bind_result(backend->mysql->odb_read_header, result_buffers) != 0)
      return GIT_ERROR;

    // this should populate the buffers at *type_p and *len_p
	if (mysql_stmt_fetch(backend->mysql->odb_read_header) != 0)
      return GIT_ERROR;

	error = GIT_OK;

	*len_p = raw.len;
	*type_p = raw.type;

  } else
    error = GIT_ENOTFOUND;

  // free result
  if (mysql_stmt_free_result(backend->mysql->odb_read_header) != 0)
      return GIT_ERROR;
  mysql_free_result(prepare_meta_result);

  // reset the statement for further use
  if (mysql_stmt_reset(backend->mysql->odb_read_header) != 0)
	  return GIT_ERROR;

  return error;
}

int mysql_odb_read(void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int error = GIT_ERROR;
  MYSQL_BIND bind_buffers[1];
  MYSQL_BIND result_buffers[3];
  MYSQL_RES  *meta_result = NULL;
  unsigned long data_len = 0;
    
  git_rawobj raw;

  assert(len_p && type_p && _backend && oid);
    
  memset(&raw, 0, sizeof(raw));

  backend = (git_mysql_odb *)_backend;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the oid passed to the statement
  bind_buffers[0].buffer = (void*)oid->id;
  bind_buffers[0].buffer_length = 20;
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

  if (mysql_stmt_bind_param(backend->mysql->odb_read, bind_buffers) != 0)
	  return GIT_ERROR;

  meta_result = mysql_stmt_result_metadata(backend->mysql->odb_read);
  if(!meta_result)
    return GIT_ERROR;

  // execute the statement
  if (mysql_stmt_execute(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_store_result(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  // this should either be 0 or 1
  if (mysql_stmt_num_rows(backend->mysql->odb_read) == 1) {

	memset(result_buffers, 0, sizeof(result_buffers));

	result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
	result_buffers[0].buffer = (void *)&raw.type;
	result_buffers[0].buffer_length = sizeof(*type_p);
	result_buffers[0].is_null = 0;
	result_buffers[0].length = &result_buffers[0].buffer_length;

  	result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
	result_buffers[1].buffer = (void *)&raw.len;
	result_buffers[1].buffer_length = sizeof(*len_p);
	result_buffers[1].is_null = 0;
	result_buffers[1].length = &result_buffers[1].buffer_length;

	data_len = meta_result->fields[2].max_length;
	raw.data = malloc(data_len);

	result_buffers[2].buffer_type = MYSQL_TYPE_LONG_BLOB;
  	result_buffers[2].buffer = raw.data;
	result_buffers[2].is_null = 0;
  	result_buffers[2].buffer_length = data_len;
  	result_buffers[2].length = &data_len;

	if (mysql_stmt_bind_result(backend->mysql->odb_read, result_buffers) != 0)
      return GIT_ERROR;

	mysql_stmt_fetch(backend->mysql->odb_read);

	error = GIT_OK;
      
    *data_p = raw.data;
    *len_p = raw.len;
    *type_p = raw.type;
      
  } else
    error = GIT_ENOTFOUND;

  mysql_free_result(meta_result);

  // reset the statement for further use
  if (mysql_stmt_reset(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  return error;
}

int mysql_odb_exists(git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int found;
  MYSQL_BIND bind_buffers[1];

  assert(_backend && oid);

  backend = (git_mysql_odb *)_backend;
  found = 0;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the oid passed to the statement
  bind_buffers[0].buffer = (void*)oid->id;
  bind_buffers[0].buffer_length = 20;
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

  if (mysql_stmt_bind_param(backend->mysql->odb_read_header, bind_buffers) != 0)
    return 0;

  // execute the statement
  if (mysql_stmt_execute(backend->mysql->odb_read_header) != 0)
	return 0;

  if (mysql_stmt_store_result(backend->mysql->odb_read_header) != 0)
	return 0;

  // now lets see if any rows matched our query
  // this should either be 0 or 1
  // if it's > 1 MySQL's unique index failed and we should all fear for our lives
  if (mysql_stmt_num_rows(backend->mysql->odb_read_header) == 1) {
    found = 1;
  }

  // free result
  if (mysql_stmt_free_result(backend->mysql->odb_read_header) != 0)
    return GIT_ERROR;

  // reset the statement for further use
  if (mysql_stmt_reset(backend->mysql->odb_read_header) != 0)
	return 0;

  return found;
}

int mysql_odb_write(git_odb_backend *_backend, const git_oid *oid, const void *data, size_t len, git_otype type)
{
  //int error;
  git_mysql_odb *backend;
  MYSQL_BIND bind_buffers[4];
  my_ulonglong affected_rows;

  assert(oid && _backend && data);

  backend = (git_mysql_odb *)_backend;

  //if ((error = git_odb_hash(oid, data, len, type)) < 0)
  //  return error;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the oid
  bind_buffers[0].buffer = (void*)oid->id;
  bind_buffers[0].buffer_length = 20;
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

  // bind the type
  bind_buffers[1].buffer = &type;
  bind_buffers[1].buffer_length = sizeof(type);
  bind_buffers[1].length = &bind_buffers[1].buffer_length;
  bind_buffers[1].buffer_type = MYSQL_TYPE_TINY;

  // bind the size of the data
  bind_buffers[2].buffer = &len;
  bind_buffers[2].buffer_length = sizeof(len);
  bind_buffers[2].length = &bind_buffers[2].buffer_length;
  bind_buffers[2].buffer_type = MYSQL_TYPE_LONG;

  // bind the data
  bind_buffers[3].buffer = (void*)data;
  bind_buffers[3].buffer_length = len;
  bind_buffers[3].length = &bind_buffers[3].buffer_length;
  bind_buffers[3].buffer_type = MYSQL_TYPE_BLOB;


  if (mysql_stmt_bind_param(backend->mysql->odb_write, bind_buffers) != 0)
    return GIT_ERROR;

  // TODO: use the streaming backend API so this actually makes sense to use :P
  // once we want to use this we should comment out
  // if (mysql_stmt_send_long_data(backend->mysql->odb_write, 4, data, len) != 0)
  //   return GIT_ERROR;

  // execute the statement
  if (mysql_stmt_execute(backend->mysql->odb_write) != 0)
    return GIT_ERROR;

  // now lets see if the insert worked
  affected_rows = mysql_stmt_affected_rows(backend->mysql->odb_write);
  if (affected_rows != 1)
    return GIT_ERROR;

  // reset the statement for further use
  if (mysql_stmt_reset(backend->mysql->odb_read_header) != 0)
    return GIT_ERROR;

  return GIT_OK;
}

void mysql_odb_free(git_odb_backend *_backend)
{
  git_mysql_odb *backend;
  assert(_backend);
  backend = (git_mysql_odb *)_backend;

  //git_mysql_free(backend->mysql);

  free(backend);
}

int git_mysql_odb_init(git_odb_backend **out, git_mysql *mysql)
{
  git_mysql_odb *mysql_odb;
  mysql_odb = calloc(1, sizeof(git_mysql_odb));

  int error;

  //git_odb_backend_malloc(mysql_odb->parent, sizeof(git_odb_backend));

  error = git_odb_init_backend(&mysql_odb->parent, GIT_ODB_BACKEND_VERSION);
  if (error < 0)
	  return GIT_ERROR;

  mysql_odb->parent.read = &mysql_odb_read;
  mysql_odb->parent.read_header = &mysql_odb_read_header;
  //mysql_odb->parent.read_prefix = &mysql_odb_read_prefix;
  mysql_odb->parent.write = &mysql_odb_write;
  mysql_odb->parent.exists = &mysql_odb_exists;
  mysql_odb->parent.free = &mysql_odb_free;

  mysql_odb->mysql = mysql;

  *out = (git_odb_backend *)mysql_odb;

  return GIT_OK;
}
