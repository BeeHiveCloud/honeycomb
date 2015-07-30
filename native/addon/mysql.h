#ifndef NODE_ADDON_MYSQL_H
#define NODE_ADDON_MYSQL_H

#include <nan.h>
#include <string>

extern "C" {
#include "../mysql/mysql.h"
}

using namespace node;
using namespace v8;

class MySQL : public ObjectWrap {
  public:
	  static Persistent<Function> constructor_template;
	  static void InitializeComponent (Handle<v8::Object> target);

	  static Handle<v8::Value> New(void *raw, bool selfFreeing);

	  bool selfFreeing;

	  static MYSQL *db;

  private:
	  void *raw;
	  const char *error;

	  // Con(De)structor
	  static NAN_METHOD(JSNewFunction);
	  MySQL(void *raw, bool selfFreeing);
	  ~MySQL();

	  // Properties
	  static NAN_GETTER(GetError);
	  static NAN_SETTER(SetError);

	  // Methods
	  static NAN_METHOD(Open);
	  static NAN_METHOD(Set);
	  static NAN_METHOD(Get);

};


#endif
