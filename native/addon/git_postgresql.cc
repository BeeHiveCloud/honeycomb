#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "git_postgresql.h"

void GitPostgreSQL::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

    Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "Open", Open);

    target->Set(NanNew<String>("PostgreSQL"), object);
}


NAN_METHOD(GitPostgreSQL::Open) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("connection string is required.");
	}

	// start convert_from_v8 block
	const char * from_conn;
	String::Utf8Value conn_buf(args[0]->ToString());
	from_conn = (const char *)strdup(*conn_buf);
	// end convert_from_v8 block

	int error;

	//error = git_pq_init(&conn, from_conn);
	//if (error < 0){
	//	return NanThrowError("git_pq_init error");
	//}

	error = git_libgit2_init();
	if (error < 0){
		return NanThrowError("git_libgit2_init error");
	}

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());

}


Persistent<Function> GitPostgreSQL::constructor_template;
PGconn * GitPostgreSQL::conn;
git_repository *GitPostgreSQL::repo;
