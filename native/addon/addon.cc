#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace node;
using namespace v8;

#include "git_mysql.h"
#include "git_postgresql.h"

void init(Handle<v8::Object> target) {
  NanScope();

  GitMysql::InitializeComponent(target);
  GitPostgreSQL::InitializeComponent(target);
}

NODE_MODULE(addon, init)
