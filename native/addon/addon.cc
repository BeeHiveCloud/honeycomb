#include "wrapper.h"
#include "mysql.h"

void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::InitializeComponent(target);

  GitMysql::InitializeComponent(target);

}

NODE_MODULE(addon, init)
