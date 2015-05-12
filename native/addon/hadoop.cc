#include <nan.h>
#include <string.h>

#include "hadoop.h"

void Hadoop::InitializeComponent(Handle<v8::Object> target) {
	NanScope();

	Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "HDFSTest", HDFSTest);

	target->Set(NanNew<String>("Hadoop"), object);
}

NAN_METHOD(Hadoop::HDFSTest) {
	NanEscapableScope();

	int error;

	error = hdfs_test();

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}