#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "git_backend.h"


void GitBackend::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

    Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "gitMysqlInit", GitMysqlInit);

	NODE_SET_METHOD(object, "gitMysqlFree", GitMysqlFree);

	NODE_SET_METHOD(object, "gitMysqlOdbInit", GitMysqlOdbInit);

    target->Set(NanNew<String>("SQL"), object);
}


NAN_METHOD(GitBackend::GitMysqlInit) {
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

  git_mysql *from_out;
  int error = git_mysql_init(&from_out,
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
  //Handle<v8::External> mysql;
  //mysql = NanNew<External>((void *)from_out);

  //GitMysql* object = new GitMysql(from_out, true);
  Handle<v8::Value> obj = GitMysql::New(from_out, true);
  //Local<GitMysql> obj = GitMysql::New(from_out, true);

  NanReturnValue(obj);
}

NAN_METHOD(GitBackend::GitMysqlFree) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsObject()) {
		return NanThrowError("mysql is required.");
	}

	// start convert_from_v8 block
	GitMysql* from_mysql;
	from_mysql = ObjectWrap::Unwrap<GitMysql>(args[0]->ToObject());

	// end convert_from_v8 block

	int result = git_mysql_free(from_mysql->GetValue());

	Handle<v8::Value> to;

	to = NanNew<Number>(result);

	NanReturnValue(to);
}

NAN_METHOD(GitBackend::GitMysqlOdbInit) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsExternal()) {
		return NanThrowError("mysql is required.");
	}

	// start convert_from_v8 block

	git_odb_backend *from_out;

	git_mysql *from_mysql;
	from_mysql = ObjectWrap::Unwrap<git_mysql>(args[1]->ToObject());

	// end convert_from_v8 block

	int result = git_mysql_odb_init(&from_out,from_mysql);

	Handle<v8::Value> to;

	to = NanNew<Number>(result);

	NanReturnValue(to);
}

Persistent<Function> GitBackend::constructor_template;
