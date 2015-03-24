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

    target->Set(NanNew<String>("SQL"), object);
}


NAN_METHOD(GitBackend::GitMysqlOpen) {
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

  error = git_repository_wrap_odb(&repo, odb_backend->odb);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_odb_new error")));
	  return Undefined();
  }

  error = git_odb_add_backend(odb_backend->odb, odb_backend, 1);
  if (error < 0){
	  ThrowException(Exception::TypeError(String::New("git_odb_add_backend error")));
	  return Undefined();
  }

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
		return NanThrowError("string is required.");
	}

	// start convert_from_v8 block
	const char *from_blob;
	String::Utf8Value blob(args[0]->ToString());
	from_blob = (const char *)strdup(*blob);
	// end convert_from_v8 block

	int				  error;
	git_oid			  oid;

	error = git_blob_create_frombuffer(&oid, repo, from_blob, strlen(from_blob));
	if (error < 0){
		ThrowException(Exception::TypeError(String::New("git_blob_create_frombuffer error")));
		return Undefined();
	}

	if (error == 0)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

Persistent<Function> GitBackend::constructor_template;
git_mysql * GitBackend::mysql;
git_repository *GitBackend::repo;
