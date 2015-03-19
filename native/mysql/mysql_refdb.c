#include "mysql_refdb.h"

int mysql_ref_iterator_next(git_reference **ref, git_reference_iterator *iter){

}

int mysql_ref_iterator_next_name(const char **ref_name, git_reference_iterator *iter){

}

int mysql_ref_iterator_free(git_reference_iterator *iter){

}

int mysql_refdb_exists(int *exists, git_refdb_backend *backend, const char *ref_name){

}

int mysql_refdb_lookup(git_reference **out, git_refdb_backend *backend, const char *ref_name){

}

int mysql_refdb_iterator(git_reference_iterator **iter, git_refdb_backend *backend, const char *glob){

}

int mysql_refdb_write(git_refdb_backend *backend,
					  const git_reference *ref, int force,
					  const git_signature *who, const char *message,
					  const git_oid *old, const char *old_target)
{

}

int mysql_refdb_rename(git_reference **out, git_refdb_backend *backend,
					   const char *old_name, const char *new_name, int force,
					   const git_signature *who, const char *message)
{

}

int mysql_refdb_del(git_refdb_backend *backend, const char *ref_name, const git_oid *old_id, const char *old_target){

}

int mysql_refdb_free(git_refdb_backend *backend){

}

int git_mysql_refdb_init(git_refdb_backend **out, git_mysql *mysql)
{
	git_mysql_refdb *mysql_refdb;
	mysql_refdb = calloc(1, sizeof(git_mysql_refdb));

	int error;

	//git_odb_backend_malloc(mysql_odb->parent, sizeof(git_odb_backend));

	error = git_odb_init_backend(&mysql_refdb->parent, GIT_REFDB_BACKEND_VERSION);
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
