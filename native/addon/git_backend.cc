#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "git_backend.h"

void GitBackend::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

    Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "gitMysqlOpen", GitMysqlOpen);

	NODE_SET_METHOD(object, "gitMysqlClose", GitMysqlClose);

	NODE_SET_METHOD(object, "gitMysqlCreateBlob", GitMysqlCreateBlob);

	NODE_SET_METHOD(object, "gitMysqlCreateTree", GitMysqlCreateTree);

    target->Set(NanNew<String>("SQL"), object);
}


extern "C" NAN_METHOD(GitBackend::GitMysqlOpen) {
  NanEscapableScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("mysql host is required.");
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("mysql user is required.");
  }

  if (args.Length() == 2 || !args[2]->IsString()) {
    return NanThrowError("mysql password is required.");
  }

  if (args.Length() == 3 || !args[3]->IsString()) {
    return NanThrowError("mysql db is required.");
  }

  if (args.Length() == 4 || !args[4]->IsNumber()) {
    return NanThrowError("mysql port required.");
  }

// start convert_from_v8 block
  const char * from_mysql_host;
  String::Utf8Value mysql_host(args[0]->ToString());
  from_mysql_host = (const char *) strdup(*mysql_host);

  const char * from_mysql_user;
  String::Utf8Value mysql_user(args[1]->ToString());
  from_mysql_user = (const char *) strdup(*mysql_user);

  const char * from_mysql_passwd;
  String::Utf8Value mysql_passwd(args[2]->ToString());
  from_mysql_passwd = (const char *) strdup(*mysql_passwd);

  const char * from_mysql_db;
  String::Utf8Value mysql_db(args[3]->ToString());
  from_mysql_db = (const char *) strdup(*mysql_db);

  unsigned int from_mysql_port;
  from_mysql_port = (unsigned int)args[4]->ToNumber()->Value();
// end convert_from_v8 block

  int error = git_mysql_init(&mysql,
							  from_mysql_host,
							  from_mysql_user,
							  from_mysql_passwd,
							  from_mysql_db,
							  from_mysql_port,
							  NULL,
							  0);

  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_mysql_init error")));
	  return Undefined();
  }

  git_odb_backend   *odb_backend;
  error = git_mysql_odb_init(&odb_backend, mysql);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_mysql_odb_init error")));
	  return Undefined();
  }

  error = git_odb_new(&odb_backend->odb);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_odb_new error")));
	  return Undefined();
  }

  error = git_odb_add_backend(odb_backend->odb, odb_backend, 1);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_odb_add_backend error")));
	  return Undefined();
  }

  error = git_repository_wrap_odb(&repo, odb_backend->odb);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_repository_wrap_odb error")));
	  return Undefined();
  }
  
  error = git_repository_set_workdir(repo, "/", 0);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_repository_set_workdir error")));
	  return Undefined();
  }
  printf("workdir:%s", git_repository_workdir(repo));

  error = git_repository_set_path(repo, "/.git");
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_repository_set_path error")));
	  return Undefined();
  } 
  printf("path:%s", git_repository_path(repo));



  //Handle<v8::Value> obj = GitMysql::New(from_out, false);
  //NanReturnValue(obj);
  if (error == 0)
	  NanReturnValue(NanTrue());
  else
	  NanReturnValue(NanFalse());

}

NAN_METHOD(GitBackend::GitMysqlClose) {
	NanEscapableScope();

	int error = git_mysql_free(mysql);

	if (error == 0)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitBackend::GitMysqlCreateBlob) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("path is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("buffer is required.");
	}

	// start convert_from_v8 block
	const char *from_path;
	String::Utf8Value path_buf(args[0]->ToString());
	from_path = (const char *)strdup(*path_buf);

	const char *from_buf;
	String::Utf8Value blob_buf(args[1]->ToString());
	from_buf = (const char *)strdup(*blob_buf);
	// end convert_from_v8 block

	int				  error;
	git_oid			  oid;

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_blob_create_frombuffer(&oid, repo, from_buf, strlen(from_buf));
	if (error < 0){
		git_mysql_rollback(mysql);
		ThrowException(Exception::TypeError(String::New("git_blob_create_frombuffer error")));
		return Undefined();
	}

	error = git_mysql_index_write(mysql, &oid, from_path);
	if (error < 0){
		git_mysql_rollback(mysql);
		ThrowException(Exception::TypeError(String::New("git_mysql_index_write error")));
		return Undefined();
	}

	git_mysql_commit(mysql);

	char sha1[GIT_OID_HEXSZ+2] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ+1, &oid);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);
}

NAN_METHOD(GitBackend::GitMysqlCreateTree) {
	NanEscapableScope();

	int				  error;
	git_oid			  oid,tree;
	git_treebuilder   *bld = NULL;

	git_oid_fromstr(&oid, "41842f42ac3734774a6192c9d7f03ad972c684fc");

	error = git_treebuilder_new(&bld, repo, NULL);
	if (error < 0){
		ThrowException(Exception::TypeError(String::New("git_treebuilder_new error")));
		return Undefined();
	}

	error = git_treebuilder_insert(NULL, bld, "README.md", &oid, GIT_FILEMODE_BLOB);
	if (error < 0){
		ThrowException(Exception::TypeError(String::New("git_treebuilder_insert error")));
		return Undefined();
	}

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_treebuilder_write(&tree, bld);
	if (error < 0){
		git_mysql_rollback(mysql);
		ThrowException(Exception::TypeError(String::New("git_treebuilder_write error")));
		return Undefined();
	}

	git_mysql_commit(mysql);

	git_treebuilder_free(bld);

	char sha1[GIT_OID_HEXSZ + 2] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, &tree);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);
}

Persistent<Function> GitBackend::constructor_template;
git_mysql * GitBackend::mysql;
git_repository *GitBackend::repo;
//const char * GitBackend::path_buf = "/.git";
