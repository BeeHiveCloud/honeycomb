#ifndef NODE_ADDON_MYSQL_H
#define NODE_ADDON_MYSQL_H

#include <nan.h>
#include <string>

using namespace node;
using namespace v8;

class Mysql : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

  private:

	  static NAN_METHOD(Open);
	  static NAN_METHOD(Close);

};


#endif
