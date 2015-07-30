#ifndef NODE_ADDON_GIT_MYSQL_H
#define NODE_ADDON_GIT_MYSQL_H

#include <nan.h>
#include <string>

extern "C" {
#include "git2.h"
#include "git2/sys/repository.h"
#include "../git/mysql/mysql_backend.h"
#include "../git/mysql/mysql_odb.h"
#include "../git/mysql/mysql_refdb.h"
#include "../git/mysql/mysql_index.h"
#include "../git/mysql/mysql_tree.h"
#include "../git/mysql/mysql_repo.h"
#include "../git/mysql/mysql_config.h"
#include "../patch/libgit2/repo_path.h"
#include "../mysql/mysql.h"
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

	  GitMysql();
	  ~GitMysql();

	  // Constructor
	  static NAN_METHOD(JSNewFunction);

	  // Properties

	  // Methods
	  static NAN_METHOD(LastError);
	  static NAN_METHOD(CreateBlob);
	  static NAN_METHOD(Commit);
	  static NAN_METHOD(CreateBranch);
	  static NAN_METHOD(CreateRepo);
	  static NAN_METHOD(DeleteRepo);
	  static NAN_METHOD(CreateTag);
	  static NAN_METHOD(Diff);
	  static NAN_METHOD(Blame);
};

#endif
