#include "mysql_refdb.h"


int mysql_ref_iterator_next(git_reference **ref, git_reference_iterator *iter){
	return GIT_OK;
}

int mysql_ref_iterator_next_name(const char **ref_name, git_reference_iterator *iter){
	return GIT_OK;
}

int mysql_ref_iterator_free(git_reference_iterator *iter){
	return GIT_OK;
}

int mysql_refdb_exists(int *exists, git_refdb_backend *_backend, const char *ref_name){
	git_mysql_refdb *backend;
	int found;
	MYSQL_BIND bind_buffers[1];

	assert(_backend && ref_name);

	backend = (git_mysql_refdb *)_backend;
	found = 0;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the name passed to the statement
	bind_buffers[0].buffer = (void *)ref_name;
	bind_buffers[0].buffer_length = strlen(ref_name);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(backend->mysql->refdb_read_header, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(backend->mysql->refdb_read_header) != 0)
		return GIT_ERROR;

	if (mysql_stmt_store_result(backend->mysql->refdb_read_header) != 0)
		return GIT_ERROR;

	// this should either be 0 or 1
	if (mysql_stmt_num_rows(backend->mysql->refdb_read_header) == 1) {
		found = 1;
	}
   
	// reset the statement for further use
	if (mysql_stmt_reset(backend->mysql->refdb_read_header) != 0)
		return 0;

	*exists = found;
	return GIT_OK;
}

int mysql_refdb_lookup(git_reference **out, git_refdb_backend *_backend, const char *ref_name){
	git_mysql_refdb *backend;
	int error;
	MYSQL_BIND bind_buffers[1];
	MYSQL_BIND result_buffers[2];

	git_ref_t type;
	git_oid target;
	char *target_buf;
	unsigned long target_len;

	assert(_backend && ref_name);

	backend = (git_mysql_refdb *)_backend;
	error = GIT_ERROR;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the name passed to the statement
	bind_buffers[0].buffer = (void *)ref_name;
	bind_buffers[0].buffer_length = strlen(ref_name);
	bind_buffers[0].length = &bind_buffers[0].buffer_length;
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (mysql_stmt_bind_param(backend->mysql->refdb_read, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(backend->mysql->refdb_read) != 0)
		return GIT_ERROR;

	if (mysql_stmt_store_result(backend->mysql->refdb_read) != 0)
		return GIT_ERROR;

	// this should either be 0 or 1
	if (mysql_stmt_num_rows(backend->mysql->refdb_read) == 1) {

		memset(result_buffers, 0, sizeof(result_buffers));

		result_buffers[0].buffer_type = MYSQL_TYPE_TINY;
		result_buffers[0].buffer = &type;
		result_buffers[0].buffer_length = sizeof(type);
		result_buffers[0].is_null = 0;
		result_buffers[0].length = &result_buffers[0].buffer_length; //&type_len
		memset(&type, 0, sizeof(type));

		result_buffers[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		result_buffers[1].buffer = 0;
		result_buffers[1].buffer_length = 0;
		result_buffers[1].is_null = 0;
		result_buffers[1].length = &target_len;

		if (mysql_stmt_bind_result(backend->mysql->refdb_read, result_buffers) != 0)
			return GIT_ERROR;

		error = mysql_stmt_fetch(backend->mysql->refdb_read);
		//if(error != 0 || error != MYSQL_DATA_TRUNCATED)
		//   return GIT_ERROR;

		if (target_len > 0){
			target_buf = calloc(1, target_len + 1);
			result_buffers[1].buffer = target_buf;
			result_buffers[1].buffer_length = target_len + 1;

			if (mysql_stmt_fetch_column(backend->mysql->refdb_read, &result_buffers[1], 1, 0) != 0)
				return GIT_ERROR;
		}

		if (type == GIT_REF_OID){
			git_oid_fromstr(&target, target_buf);
			*out = git_reference__alloc(ref_name, &target, NULL);
		}
		else if (type == GIT_REF_SYMBOLIC) {
			*out = git_reference__alloc_symbolic(ref_name, target_buf);
		}
		else {
			giterr_set_str(GITERR_REFERENCE, "unknown ref type returned");
			error = GIT_ERROR;
		}

		error = GIT_OK;
	}
	else 
		error = GIT_ENOTFOUND;
    
	// reset the statement for further use
	if (mysql_stmt_reset(backend->mysql->refdb_read) != 0)
		return GIT_ERROR;


	return error;
}

int mysql_refdb_iterator(git_reference_iterator **iter, git_refdb_backend *backend, const char *glob){
	return GIT_OK;
}

int mysql_refdb_write(git_refdb_backend *_backend,
					  const git_reference *ref, int force,
					  const git_signature *who, const char *message,
					  const git_oid *old, const char *old_target)
{
	//int error;
	git_mysql_refdb *backend;
	MYSQL_BIND bind_buffers[3];
	my_ulonglong affected_rows;

	const char *ref_name = git_reference_name(ref);
	const git_ref_t ref_type = git_reference_type(ref);
	const git_oid *target = git_reference_target(ref);
	const char *symbolic_target = git_reference_symbolic_target(ref);

	assert(ref && _backend);

	backend = (git_mysql_refdb *)_backend;

	memset(bind_buffers, 0, sizeof(bind_buffers));

	// bind the name
	bind_buffers[0].buffer = (void *)ref_name;
	bind_buffers[0].buffer_length = strlen(ref_name);
	bind_buffers[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	
	// bind the type
	bind_buffers[1].buffer = (void *)&ref_type;
	bind_buffers[1].buffer_length = sizeof(ref_type);
	bind_buffers[1].buffer_type = MYSQL_TYPE_TINY;

	// bind the target
	bind_buffers[2].buffer_type = MYSQL_TYPE_VAR_STRING;

	if (ref_type == GIT_REF_OID){
		char sha1[GIT_OID_HEXSZ + 1] = { 0 };
		git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, target);

		bind_buffers[2].buffer = sha1;
		bind_buffers[2].buffer_length = GIT_OID_HEXSZ;
		bind_buffers[2].length = &bind_buffers[2].buffer_length;
	}
	else if (ref_type == GIT_REF_SYMBOLIC) {
		bind_buffers[2].buffer = (void *)symbolic_target;
		bind_buffers[2].buffer_length = strlen(symbolic_target);
		bind_buffers[2].length = &bind_buffers[2].buffer_length;
	}
	else {
		giterr_set_str(GITERR_REFERENCE, "unknown ref type returned");
		return GIT_ERROR;
	}


	if (mysql_stmt_bind_param(backend->mysql->refdb_write, bind_buffers) != 0)
		return GIT_ERROR;

	// execute the statement
	if (mysql_stmt_execute(backend->mysql->refdb_write) != 0)
		return GIT_ERROR;

	affected_rows = mysql_stmt_affected_rows(backend->mysql->refdb_write);
	//if (affected_rows != 1)
	//	return GIT_ERROR;

	// reset the statement for further use
	if (mysql_stmt_reset(backend->mysql->refdb_write) != 0)
		return GIT_ERROR;

	return GIT_OK;
}

int mysql_refdb_rename(git_reference **out, git_refdb_backend *backend,
					   const char *old_name, const char *new_name, int force,
					   const git_signature *who, const char *message)
{
	return GIT_OK;
}

int mysql_refdb_del(git_refdb_backend *backend, const char *ref_name, const git_oid *old_id, const char *old_target){
	return GIT_OK;
}

void mysql_refdb_free(git_refdb_backend *backend){

}

int git_mysql_refdb_init(git_refdb_backend **out, git_mysql *mysql)
{
	git_mysql_refdb *mysql_refdb;
	mysql_refdb = calloc(1, sizeof(git_mysql_refdb));

	int error;

	//git_odb_backend_malloc(mysql_odb->parent, sizeof(git_odb_backend));

	error = git_refdb_init_backend(&mysql_refdb->parent, GIT_REFDB_BACKEND_VERSION);
	if (error < 0)
		return GIT_ERROR;

	mysql_refdb->parent.exists = &mysql_refdb_exists;
	mysql_refdb->parent.lookup = &mysql_refdb_lookup;
	mysql_refdb->parent.iterator = &mysql_refdb_iterator;
	mysql_refdb->parent.write = &mysql_refdb_write;
	mysql_refdb->parent.rename = &mysql_refdb_rename;
	mysql_refdb->parent.del = &mysql_refdb_del;
	mysql_refdb->parent.free = &mysql_refdb_free;

	mysql_refdb->mysql = mysql;

	*out = (git_refdb_backend *)mysql_refdb;

	return GIT_OK;
}
