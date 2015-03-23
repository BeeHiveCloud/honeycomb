#ifndef NODE_ADDON_GIT_BACKEND_H
#define NODE_ADDON_GIT_BACKEND_H

#include <nan.h>
#include <string>

extern "C" {
#include "../mysql/mysql_backend.h"
#include "../mysql/mysql_odb.h"
}

#include "git_mysql.h"

using namespace node;
using namespace v8;

class GitBackend : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    bool selfFreeing;

  private:

    static NAN_METHOD(JSNewFunction);

    static NAN_METHOD(GitMysqlInit);

	static NAN_METHOD(GitMysqlFree);

	static NAN_METHOD(GitMysqlOdbInit);
};

#endif
