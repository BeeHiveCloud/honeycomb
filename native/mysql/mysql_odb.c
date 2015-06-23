#include "mysql_odb.h"

/*
int mysql_odb_read_prefix(	git_oid *out_oid,
							void **data_p,
							size_t *len_p,
							git_otype *type_p,
							git_odb_backend *_backend,
							const git_oid *short_oid,
							size_t len)
{
	if (len >= GIT_OID_HEXSZ) {
		int error = mysql_odb_read(data_p, len_p, type_p, _backend, short_oid);
		if (error == GIT_OK)
			git_oid_cpy(out_oid, short_oid);

		return error;
	}
	else{
		giterr_set_str(GITERR_ODB, "mysql odb doesn't not implement oid prefix lookup");
		return GITERR_INVALID;
	}
}
*/

int mysql_odb_read_header(size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int error = GIT_ERROR;
  MYSQL_BIND bind_buffers[2];
  MYSQL_BIND result_buffers[2];
  MYSQL_RES  *prepare_meta_result = NULL;
  //my_bool       is_null[2];
  //my_bool       my_error[2];

  assert(len_p && type_p && _backend && oid);

  backend = (git_mysql_odb *)_backend;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the repo passed to the statement
  bind_buffers[0].buffer = (void *)&(backend->mysql->repo);
  bind_buffers[0].buffer_length = sizeof(backend->mysql->repo);
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

  // bind the oid passed to the statement
  bind_buffers[1].buffer = (void*)oid->id;
  bind_buffers[1].buffer_length = 20;
  bind_buffers[1].length = &bind_buffers[1].buffer_length;
  bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

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
    result_buffers[0].buffer = type_p;
	result_buffers[0].buffer_length = sizeof(*type_p);
	result_buffers[0].is_null = 0; // &is_null[0];
	//result_buffers[0].error = &my_error[0];
	result_buffers[0].length = &result_buffers[0].buffer_length;
	memset(type_p, 0, sizeof(*type_p));

    result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
    result_buffers[1].buffer = len_p;
	result_buffers[1].buffer_length = sizeof(*len_p);
	result_buffers[1].is_null = 0; // &is_null[1];
	//result_buffers[1].error = &my_error[1];
	result_buffers[1].length = &result_buffers[1].buffer_length;
	memset(len_p, 0, sizeof(*len_p));

	if (mysql_stmt_bind_result(backend->mysql->odb_read_header, result_buffers) != 0)
      return GIT_ERROR;

    // this should populate the buffers at *type_p and *len_p
	if (mysql_stmt_fetch(backend->mysql->odb_read_header) != 0)
      return GIT_ERROR;

	error = GIT_OK;
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
  MYSQL_BIND bind_buffers[2];
  MYSQL_BIND result_buffers[3];
  MYSQL_RES  *prepare_meta_result = NULL;
  unsigned long data_len = 0;
  my_bool truth = 1;

  assert(len_p && type_p && _backend && oid);

  backend = (git_mysql_odb *)_backend;

  if (mysql_stmt_attr_set(backend->mysql->odb_read, STMT_ATTR_UPDATE_MAX_LENGTH, &truth) != 0)
    return GIT_ERROR;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the repo passed to the statement
  bind_buffers[0].buffer = &(backend->mysql->repo);
  bind_buffers[0].buffer_length = sizeof(backend->mysql->repo);
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

  // bind the oid passed to the statement
  bind_buffers[1].buffer = (void*)oid->id;
  bind_buffers[1].buffer_length = 20;
  bind_buffers[1].length = &bind_buffers[1].buffer_length;
  bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

  if (mysql_stmt_bind_param(backend->mysql->odb_read, bind_buffers) != 0)
	  return GIT_ERROR;

  prepare_meta_result = mysql_stmt_result_metadata(backend->mysql->odb_read);
  if(!prepare_meta_result)
    return GIT_ERROR;

  MYSQL_FIELD* data_field = &prepare_meta_result->fields[2];

  printf("max length:%d\n",data_field->max_length);

  // execute the statement
  if (mysql_stmt_execute(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  if (mysql_stmt_store_result(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  // this should either be 0 or 1
  // if it's > 1 MySQL's unique index failed and we should all fear for our lives
  if (mysql_stmt_num_rows(backend->mysql->odb_read) == 1) {

	memset(result_buffers, 0, sizeof(result_buffers));

	result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
	result_buffers[0].buffer = type_p;
	result_buffers[0].buffer_length = sizeof(*type_p);
	result_buffers[0].is_null = 0;
	result_buffers[0].length = &result_buffers[0].buffer_length;
	memset(type_p, 0, sizeof(*type_p));

  	result_buffers[1].buffer_type = MYSQL_TYPE_LONG;
	result_buffers[1].buffer = len_p;
	result_buffers[1].buffer_length = sizeof(*len_p);
	result_buffers[1].is_null = 0;
	result_buffers[1].length = &result_buffers[1].buffer_length;
        //printf("len_p:%d\n",*len_p);
	memset(len_p, 0, sizeof(*len_p));

    // by setting buffer and buffer_length to 0, this tells libmysql
    // we want it to set data_len to the *actual* length of that field
    // this way we can malloc exactly as much memory as we need for the buffer
    //
    // come to think of it, we can probably just use the length set in *len_p
    // once we fetch the result?

	result_buffers[2].buffer_type = MYSQL_TYPE_LONG_BLOB;
  	result_buffers[2].buffer = 0;
	result_buffers[2].is_null = 0;
  	result_buffers[2].buffer_length = 0;
  	result_buffers[2].length = &data_len;
	//memset(&data_len, 0, sizeof(data_len));

	if (mysql_stmt_bind_result(backend->mysql->odb_read, result_buffers) != 0)
      return GIT_ERROR;

    // this should populate the buffers at *type_p, *len_p and &data_len
	error = mysql_stmt_fetch(backend->mysql->odb_read);
    // if(error != 0 || error != MYSQL_DATA_TRUNCATED)
    //   return GIT_ERROR;

    if (data_len > 0) {
      *data_p = malloc(data_len);
      if(*data_p){
	result_buffers[2].buffer_type = MYSQL_TYPE_LONG_BLOB;
        result_buffers[2].buffer = *data_p;
	result_buffers[2].is_null = 0;
        result_buffers[2].buffer_length = data_len;
	result_buffers[2].length = &data_len;

	  if (mysql_stmt_fetch_column(backend->mysql->odb_read, &result_buffers[2], 2, 0) != 0)
             return GIT_ERROR;
      }
      else{
        printf("odb_read, malloc returned NULL\n");
        return GITERR_NOMEMORY;
      }
    }

	error = GIT_OK;
  } else
    error = GIT_ENOTFOUND;

  // free result
  if (mysql_stmt_free_result(backend->mysql->odb_read) != 0)
      return GIT_ERROR;

  mysql_free_result(prepare_meta_result);

  // reset the statement for further use
  if (mysql_stmt_reset(backend->mysql->odb_read) != 0)
	  return GIT_ERROR;

  return error;
}

int mysql_odb_exists(git_odb_backend *_backend, const git_oid *oid)
{
  git_mysql_odb *backend;
  int found;
  MYSQL_BIND bind_buffers[2];

  assert(_backend && oid);

  backend = (git_mysql_odb *)_backend;
  found = 0;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the repo passed to the statement
  bind_buffers[0].buffer = &(backend->mysql->repo);
  bind_buffers[0].buffer_length = sizeof(backend->mysql->repo);
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

  // bind the oid passed to the statement
  bind_buffers[1].buffer = (void*)oid->id;
  bind_buffers[1].buffer_length = 20;
  bind_buffers[1].length = &bind_buffers[1].buffer_length;
  bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

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
  MYSQL_BIND bind_buffers[5];
  my_ulonglong affected_rows;

  assert(oid && _backend && data);

  backend = (git_mysql_odb *)_backend;

  //if ((error = git_odb_hash(oid, data, len, type)) < 0)
  //  return error;

  memset(bind_buffers, 0, sizeof(bind_buffers));

  // bind the repo passed to the statement
  bind_buffers[0].buffer = &(backend->mysql->repo);
  bind_buffers[0].buffer_length = sizeof(backend->mysql->repo);
  bind_buffers[0].length = &bind_buffers[0].buffer_length;
  bind_buffers[0].buffer_type = MYSQL_TYPE_LONGLONG;

  // bind the oid
  bind_buffers[1].buffer = (void*)oid->id;
  bind_buffers[1].buffer_length = 20;
  bind_buffers[1].length = &bind_buffers[1].buffer_length;
  bind_buffers[1].buffer_type = MYSQL_TYPE_BLOB;

  // bind the type
  bind_buffers[2].buffer = &type;
  bind_buffers[2].buffer_length = sizeof(type);
  bind_buffers[2].length = &bind_buffers[2].buffer_length;
  bind_buffers[2].buffer_type = MYSQL_TYPE_TINY;

  // bind the size of the data
  bind_buffers[3].buffer = &len;
  bind_buffers[3].buffer_length = sizeof(len);
  bind_buffers[3].length = &bind_buffers[3].buffer_length;
  bind_buffers[3].buffer_type = MYSQL_TYPE_LONG;

  // bind the data
  bind_buffers[4].buffer = (void*)data;
  bind_buffers[4].buffer_length = len;
  bind_buffers[4].length = &bind_buffers[4].buffer_length;
  bind_buffers[4].buffer_type = MYSQL_TYPE_BLOB;


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
