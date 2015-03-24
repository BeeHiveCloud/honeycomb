#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace node;
using namespace v8;

#include "git_backend.h"

void init(Handle<v8::Object> target) {
  NanScope();

  GitBackend::InitializeComponent(target);
}

NODE_MODULE(addon, init)
