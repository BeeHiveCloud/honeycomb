#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "hbase.h"

void NodeHBase::InitializeComponent(Handle<v8::Object> target){
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>();

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("HBase"));

  NODE_SET_METHOD(tpl, "Connect", Connect);
  NODE_SET_METHOD(tpl, "DisConnect", DisConnect);
  //NODE_SET_METHOD(tpl, "GetRow", GetRow);
  NODE_SET_METHOD(tpl, "PutRow", PutRow);

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("HBase"), _constructor_template);
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
