#ifndef NODE_ADDON_GIT_MYSQL_H
#define NODE_ADDON_GIT_MYSQL_H

#include <nan.h>
#include <string>

extern "C" {
#include "git2.h"
#include "../mysql/mysql_backend.h"
#include "../mysql/mysql_odb.h"
#include "../mysql/mysql_refdb.h"
#include "../mysql/mysql_index.h"
#include "../patch/repo_path.h"
#include "git2/sys/repository.h"
}

using namespace node;
using namespace v8;

class GitMysql : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

	static git_mysql *mysql;
	static git_repository *repo;

  private:

	  static NAN_METHOD(Open);

	  static NAN_METHOD(Close);

	  static NAN_METHOD(CreateBlob);

	  static NAN_METHOD(WriteTree);

	  static NAN_METHOD(Lookup);

	  static NAN_METHOD(CreateRef);

	  static NAN_METHOD(Commit);

	  static NAN_METHOD(CreateBranch);
};

#endif
