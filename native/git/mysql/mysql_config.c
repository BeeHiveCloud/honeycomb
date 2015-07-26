#include "mysql_config.h"

int mysql_config_open(git_config_backend *_backend, git_config_level_t level){
	return GIT_OK;
}

int mysql_config_snapshot(git_config_backend ** out, struct git_config_backend *in){

	*out = in;
	return GIT_OK;
}

int mysql_config_get(git_config_backend *_backend, const char *key, git_config_entry **entry){
	git_mysql_config *backend;
	int error = GIT_ERROR;
	MYSQL_BIND bind_buffers[1];
	MYSQL_BIND result_buffers[1];

	assert(key && entry && _backend );

	backend = (git_mysql_config *)_backend;
	error = GIT_ERROR;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the key passed to the statement
	bind_buffers[0].buffer = (void *)key;
	bind_buffers[0].buffer_length = strlen(key);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(backend->mysql->config_get, bind_buffers))
		return GIT_ERROR;

	if (mysql_stmt_execute(backend->mysql->config_get))
		return GIT_ERROR;

	if (mysql_stmt_store_result(backend->mysql->config_get))
		return GIT_ERROR;

	if (mysql_stmt_num_rows(backend->mysql->odb_read_header) == 1) {

		memset(result_buffers, 0, sizeof(result_buffers));
        
        char *value = malloc(backend->mysql->meta_config->fields[0].max_length + 1);
		result_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[0].buffer_length = backend->mysql->meta_config->fields[0].max_length + 1;
		result_buffers[0].buffer = value;
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length;

		if (mysql_stmt_bind_result(backend->mysql->config_get, result_buffers))
			return GIT_ERROR;

		mysql_stmt_fetch(backend->mysql->config_get);

		git_config_entry ce;

		ce.name = key;
		ce.value = value;
		ce.level = GIT_CONFIG_LEVEL_LOCAL;

		*entry = &ce;

		error = GIT_OK;
	}
	else {
		*entry = NULL;
		error = GIT_ENOTFOUND;
	}

	return error;
}

int mysql_config_set(git_config_backend *_backend, const char *key, const char *value){
	git_mysql_config *backend;

	MYSQL_BIND bind_buffers[2];

	assert(key && value && _backend);

	backend = (git_mysql_config *)_backend;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the key passed to the statement
	bind_buffers[0].buffer = (void *)key;
	bind_buffers[0].buffer_length = strlen(key);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	// bind the value passed to the statement
	bind_buffers[1].buffer = (void *)value;
	bind_buffers[1].buffer_length = strlen(value);
	bind_buffers[1].length = &bind_buffers[1].buffer_length;
	bind_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(backend->mysql->config_set, bind_buffers))
		return GIT_ERROR;

	if (mysql_stmt_execute(backend->mysql->config_set) != 0)
		return GIT_ERROR;

	return GIT_OK;
}

int mysql_config_del(git_config_backend *_backend, const char *key){
	git_mysql_config *backend;

	MYSQL_BIND bind_buffers[1];

	assert(key && _backend);

	backend = (git_mysql_config *)_backend;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the key passed to the statement
	bind_buffers[0].buffer = (void *)key;
	bind_buffers[0].buffer_length = strlen(key);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(backend->mysql->config_del, bind_buffers))
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(backend->mysql->config_del))
		return GIT_ERROR;

	return GIT_OK;
}

void mysql_config_free(git_config_backend *_backend){
	git_mysql_config *backend;
	assert(_backend);
	backend = (git_mysql_config *)_backend;

	free(backend);
}

int git_mysql_config_init(git_config_backend **out, git_mysql *mysql)
{
	git_mysql_config *mysql_config;
	mysql_config = calloc(1, sizeof(git_mysql_config));

	int error;

	error = git_config_init_backend(&mysql_config->parent, GIT_CONFIG_BACKEND_VERSION);
	if (error < 0)
		return GIT_ERROR;

	mysql_config->parent.open = &mysql_config_open;
	mysql_config->parent.get = &mysql_config_get;
	mysql_config->parent.set = &mysql_config_set;
	mysql_config->parent.del = &mysql_config_del;
	mysql_config->parent.snapshot = &mysql_config_snapshot;
	mysql_config->parent.free = &mysql_config_free;

	mysql_config->mysql = mysql;

	*out = (git_config_backend *)mysql_config;

	return GIT_OK;
}