#include "mysql_odb.h"

int mysql_odb_read_header(size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int error = GIT_ERROR;
  MYSQL_BIND bind_buffers[1];
  MYSQL_BIND result_buffers[2];
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

  if (mysql_stmt_bind_param(backend->mysql->odb_read_header, bind_buffers))
	  return GIT_ERROR;

  if (mysql_stmt_execute(backend->mysql->odb_read_header))
	  return GIT_ERROR;

  if (mysql_stmt_store_result(backend->mysql->odb_read_header))
	  return GIT_ERROR;

  if (mysql_stmt_num_rows(backend->mysql->odb_read_header) == 1) {

	memset(result_buffers, 0, sizeof(result_buffers));

	result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
	result_buffers[0].buffer = (void *)&raw.type;
	result_buffers[0].buffer_length = sizeof(raw.type);
	result_buffers[0].is_null = 0;
	result_buffers[0].length = &result_buffers[0].buffer_length;

    result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
	result_buffers[1].buffer = (void *)&raw.len;
	result_buffers[1].buffer_length = sizeof(raw.len);
	result_buffers[1].is_null = 0;
	result_buffers[1].length = &result_buffers[1].buffer_length;

	if (mysql_stmt_bind_result(backend->mysql->odb_read_header, result_buffers))
      return GIT_ERROR;

	mysql_stmt_fetch(backend->mysql->odb_read_header);

	error = GIT_OK;

	*len_p = raw.len;
	*type_p = raw.type;

  } else
    error = GIT_ENOTFOUND;

  return error;
}

int mysql_odb_read(void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int error = GIT_ERROR;
  MYSQL_BIND bind_buffers[1];
  MYSQL_BIND result_buffers[3];
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

  if (mysql_stmt_bind_param(backend->mysql->odb_read, bind_buffers))
	  return GIT_ERROR;

  if (mysql_stmt_execute(backend->mysql->odb_read))
	  return GIT_ERROR;

  if (mysql_stmt_store_result(backend->mysql->odb_read))
	  return GIT_ERROR;

  if (mysql_stmt_num_rows(backend->mysql->odb_read) == 1) {

	memset(result_buffers, 0, sizeof(result_buffers));

	result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
	result_buffers[0].buffer = (void *)&raw.type;
	result_buffers[0].buffer_length = sizeof(raw.type);
	result_buffers[0].is_null = 0;
	result_buffers[0].length = &result_buffers[0].buffer_length;

  	result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
	result_buffers[1].buffer = (void *)&raw.len;
	result_buffers[1].buffer_length = sizeof(raw.len);
	result_buffers[1].is_null = 0;
	result_buffers[1].length = &result_buffers[1].buffer_length;

	raw.data = malloc(backend->mysql->meta_odb->fields[2].max_length);
	result_buffers[2].buffer_type = MYSQL_TYPE_LONG_BLOB;
  	result_buffers[2].buffer = raw.data;
	result_buffers[2].is_null = 0;
	result_buffers[2].buffer_length = backend->mysql->meta_odb->fields[2].max_length;
	result_buffers[2].length = &result_buffers[2].buffer_length;

	if (mysql_stmt_bind_result(backend->mysql->odb_read, result_buffers))
      return GIT_ERROR;

	mysql_stmt_fetch(backend->mysql->odb_read);

	error = GIT_OK;
      
    *data_p = raw.data;
    *len_p = raw.len;
    *type_p = raw.type;
      
  } else
    error = GIT_ENOTFOUND;

  return error;
}

int mysql_odb_exists(git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int found = 0;
  MYSQL_BIND bind_buffers[1];

  assert(_backend && oid);

  backend = (git_mysql_odb *)_backend;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the oid passed to the statement
  bind_buffers[0].buffer = (void*)oid->id;
  bind_buffers[0].buffer_length = 20;
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_BLOB;

  if (mysql_stmt_bind_param(backend->mysql->odb_read_header, bind_buffers))
    return 0;

  if (mysql_stmt_execute(backend->mysql->odb_read_header))
	return 0;

  if (mysql_stmt_store_result(backend->mysql->odb_read_header))
	return 0;

  if (mysql_stmt_num_rows(backend->mysql->odb_read_header) == 1)
    found = 1;

  return found;
}

int mysql_odb_write(git_odb_backend *_backend, const git_oid *oid, const void *data, size_t len, git_otype type)
{
  git_mysql_odb *backend;
  MYSQL_BIND bind_buffers[4];

  assert(oid && _backend && data);

  backend = (git_mysql_odb *)_backend;

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

  if (mysql_stmt_bind_param(backend->mysql->odb_write, bind_buffers))
    return GIT_ERROR;

  if (mysql_stmt_execute(backend->mysql->odb_write))
    return GIT_ERROR;

  return GIT_OK;
}

void mysql_odb_free(git_odb_backend *_backend)
{
  git_mysql_odb *backend;
    
  assert(_backend);
    
  backend = (git_mysql_odb *)_backend;

  free(backend);
}

int git_mysql_odb_init(git_odb_backend **out, git_mysql *mysql)
{
  int error = GIT_ERROR;
  git_mysql_odb *mysql_odb = NULL;
    
  mysql_odb = calloc(1, sizeof(git_mysql_odb));

  error = git_odb_init_backend(&mysql_odb->parent, GIT_ODB_BACKEND_VERSION);
  if (error < 0)
	  return GIT_ERROR;

  mysql_odb->parent.read = &mysql_odb_read;
  mysql_odb->parent.read_header = &mysql_odb_read_header;
  mysql_odb->parent.write = &mysql_odb_write;
  mysql_odb->parent.exists = &mysql_odb_exists;
  mysql_odb->parent.free = &mysql_odb_free;

  mysql_odb->mysql = mysql;

  *out = (git_odb_backend *)mysql_odb;

  return GIT_OK;
}
