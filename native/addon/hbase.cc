#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "hbase.h"

void NodeHBase::InitializeComponent(Handle<v8::Object> target){
  NanScope();

  Local<Object> object = NanNew<Object>();

  NODE_SET_METHOD(object, "Connect", Connect);
  NODE_SET_METHOD(object, "DisConnect", DisConnect);
  //NODE_SET_METHOD(object, "GetRow", GetRow);
  NODE_SET_METHOD(object, "PutRow", PutRow);

  target->Set(NanNew<String>("HBase"), object);
}

NAN_METHOD(NodeHBase::Connect) {
  NanEscapableScope();

  if(g_ho.connect("localhost", 9090))
    NanReturnValue(NanTrue());
  else
    NanReturnValue(NanFalse());
}

NAN_METHOD(NodeHBase::DisConnect) {
  NanEscapableScope();

  if(g_ho.disconnect())
    NanReturnValue(NanTrue());
  else
    NanReturnValue(NanFalse());
}

NAN_METHOD(NodeHBase::PutRow) {
  NanEscapableScope();

  if(g_ho.putRow("test","row5","data:5","value5"))
    NanReturnValue(NanTrue());
  else
    NanReturnValue(NanFalse());
}

Persistent<Function> NodeHBase::constructor_template;
CHbaseOperate NodeHBase::g_ho;
