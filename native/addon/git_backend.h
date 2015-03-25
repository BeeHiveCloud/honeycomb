#ifndef NODE_ADDON_GIT_BACKEND_H
#define NODE_ADDON_GIT_BACKEND_H

#include <nan.h>
#include <string>

extern "C" {
#include "git2.h"
#include "../mysql/mysql_backend.h"
#include "../mysql/mysql_odb.h"
#include "../mysql/mysql_index.h"
#include "../mysql/mysql_repo.h"
}

using namespace node;
using namespace v8;

class GitBackend : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    //bool selfFreeing;

	static git_mysql *mysql;
	static git_repository *repo;
	//static const char *path_buf;

  private:

    static NAN_METHOD(JSNewFunction);

    static NAN_METHOD(GitMysqlOpen);

	static NAN_METHOD(GitMysqlClose);

	static NAN_METHOD(GitMysqlCreateBlob);

	static NAN_METHOD(GitMysqlCreateTree);
};

#endif
