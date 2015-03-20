#ifndef GIT_ADDON_MYSQL_H
#define GIT_ADDON_MYSQL_H

#include <nan.h>
#include <string>

extern "C" {
#include "../mysql/mysql_backend.h"
}

using namespace node;
using namespace v8;

class GitMysql : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    bool selfFreeing;

  private:

    static NAN_METHOD(JSNewFunction);

    static NAN_METHOD(GitMysqlInit);

	//static NAN_METHOD(GitMysqlFree);
};

#endif
