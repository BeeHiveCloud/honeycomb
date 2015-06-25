#include "mysql.h"

void Mysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "Open", Open);

	NODE_SET_METHOD(object, "Close", Close);

	target->Set(NanNew<String>("MySQL"), object);
}
