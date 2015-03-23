#ifndef NODE_ADDON_MYSQL_H
#define NODE_ADDON_MYSQL_H

#include <nan.h>
#include <string>

extern "C" {
  #include <mysql.h>

#include "../mysql/mysql_backend.h"

 }

using namespace node;
using namespace v8;

class GitMysql : public ObjectWrap {
  public:
    GitMysql(git_mysql *raw, bool selfFreeing);
    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    git_mysql *GetValue();
    git_mysql **GetRefValue();
    void ClearValue();

    static Handle<v8::Value> New(void *raw, bool selfFreeing);

    bool selfFreeing;


  private:
	GitMysql();
	~GitMysql();

    void ConstructFields();

    static NAN_METHOD(JSNewFunction);

    git_mysql *raw;
};

#endif
