#ifndef NODE_ADDON_HBASE_H
#define NODE_ADDON_HBASE_H

#include <nan.h>
#include <string>
#include <mutex>

#include "../hbase/HbaseOperate.h"

using namespace node;
using namespace v8;

class NodeHBase : public ObjectWrap{
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

    static CHbaseOperate g_ho;

  private:

    static NAN_METHOD(Connect);
    static NAN_METHOD(DisConnect);
    static NAN_METHOD(PutRow);
    static NAN_METHOD(GetRow);
};

#endif
