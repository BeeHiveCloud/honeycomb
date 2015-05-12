#ifndef NODE_ADDON_HADOOP_H
#define NODE_ADDON_HADOOP_H

#include <nan.h>
#include <string>

using namespace node;
using namespace v8;


extern "C" {
#include"../hadoop/hdfs_test.h"
}


class Hadoop : public ObjectWrap {
public:

	static Persistent<Function> constructor_template;
	static void InitializeComponent(Handle<v8::Object> target);

private:

	static NAN_METHOD(HDFSTest);
};

#endif
