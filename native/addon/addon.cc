#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace node;
using namespace v8;

#include "git_mysql.h"
#include "mysql.h"
//#include "hbase.h"
//#include "rconsole.h"

void init(Handle<v8::Object> target) {
    NanScope();
    GitMysql::InitializeComponent(target);
    Mysql::InitializeComponent(target);
  //NodeHBase::InitializeComponent(target);
  //RConsole::InitializeComponent(target);
}

NODE_MODULE(addon, init)
