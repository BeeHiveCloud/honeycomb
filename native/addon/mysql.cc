#include "mysql.h"

void Mysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "Open", Open);

	NODE_SET_METHOD(object, "Close", Close);
  
  NODE_SET_METHOD(object, "Set", Set);

	target->Set(NanNew<String>("MySQL"), object);
}

NAN_METHOD(Mysql::Open) {
  NanEscapableScope();
}

NAN_METHOD(Mysql::Close) {
  NanEscapableScope();
}

NAN_METHOD(Mysql::Set) {
  NanEscapableScope();
}
