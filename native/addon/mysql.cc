// This is a generated file, modify: generate/templates/class_content.cc.
#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "mysql.h"

#include <iostream>

using namespace std;
using namespace v8;
using namespace node;


  void GitMysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

    Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "gitMysqlInit", GitMysqlInit);

	//NODE_SET_METHOD(object, "gitMysqlFree", GitMysqlFree);

    target->Set(NanNew<String>("Mysql"), object);
  }


NAN_METHOD(GitMysql::GitMysqlInit) {
  NanEscapableScope();

  if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("mysql out is required.");
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("mysql host is required.");
  }

  if (args.Length() == 2 || !args[2]->IsString()) {
    return NanThrowError("mysql user is required.");
  }

  if (args.Length() == 3 || !args[3]->IsString()) {
    return NanThrowError("mysql password is required.");
  }

  if (args.Length() == 4 || !args[4]->IsString()) {
    return NanThrowError("mysql db is required.");
  }

  if (args.Length() == 5 || !args[5]->IsNumber()) {
    return NanThrowError("mysql port required.");
  }

  if (args.Length() == 6 || !args[6]->IsString()) {
    return NanThrowError("mysql socket is required.");
  }

  if (args.Length() == 7 || !args[7]->IsNumber()) {
	  return NanThrowError("mysql flag required.");
  }

// start convert_from_v8 block
  git_mysql **from_out;
  *from_out = ObjectWrap::Unwrap<git_mysql>(args[0]->ToObject());

  const char * from_mysql_host;
  String::Utf8Value mysql_host(args[1]->ToString());
  from_mysql_host = (const char *) strdup(*mysql_host);

  const char * from_mysql_user;
  String::Utf8Value mysql_user(args[2]->ToString());
  from_mysql_user = (const char *) strdup(*mysql_user);

  const char * from_mysql_passwd;
  String::Utf8Value mysql_passwd(args[3]->ToString());
  from_mysql_passwd = (const char *) strdup(*mysql_passwd);

  const char * from_mysql_db;
  String::Utf8Value mysql_db(args[4]->ToString());
  from_mysql_db = (const char *) strdup(*mysql_db);

  unsigned int from_mysql_port;
  from_mysql_port = (unsigned int)args[5]->ToNumber()->Value();

  const char * from_mysql_unix_socket;
  String::Utf8Value mysql_unix_socket(args[6]->ToString());
  from_mysql_unix_socket = (const char *)strdup(*mysql_unix_socket);

  unsigned int from_mysql_client_flag;
  from_mysql_client_flag = (unsigned int)args[7]->ToNumber()->Value();

// end convert_from_v8 block

  int result = git_mysql_init(from_out,
							  from_mysql_host,
							  from_mysql_user,
							  from_mysql_passwd,
							  from_mysql_db,
							  from_mysql_port,
							  from_mysql_unix_socket,
							  from_mysql_client_flag);


  Handle<v8::Value> to;

  to = NanNew<Number>(result);

  NanReturnValue(to);
}

Persistent<Function> GitMysql::constructor_template;
