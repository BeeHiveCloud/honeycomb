#ifndef NODE_ADDON_GIT_BACKEND_H
#define NODE_ADDON_GIT_BACKEND_H

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

class GitBackend : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

	static git_mysql *mysql;
	static git_repository *repo;

  private:

    static NAN_METHOD(GitMysqlOpen);

	static NAN_METHOD(GitMysqlClose);

	static NAN_METHOD(GitMysqlCreateBlob);

	static NAN_METHOD(GitMysqlWriteTree);

	static NAN_METHOD(GitMysqlLookup);

	static NAN_METHOD(GitMysqlCreateRef);

	static NAN_METHOD(GitMysqlCommit);

	static NAN_METHOD(GitMysqlCreateBranch);
};

#endif
