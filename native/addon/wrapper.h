#ifndef GIT_ADDON_WRAPPER_H
#define GIT_ADDON_WRAPPER_H

#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace node;
using namespace v8;

class Wrapper : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    void *GetValue();
    static Handle<v8::Value> New(void *raw);

  private:
    Wrapper(void *raw);

    static NAN_METHOD(JSNewFunction);
    static NAN_METHOD(ToBuffer);

    void *raw;
};

#endif
