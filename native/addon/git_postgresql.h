#ifndef NODE_ADDON_GIT_POSTGRESQL_H
#define NODE_ADDON_GIT_POSTGRESQL_H

#include <nan.h>
#include <string>

extern "C" {
#include "git2.h"
#include "../postgresql/pq_backend.h"
}

using namespace node;
using namespace v8;

class GitPostgreSQL : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void InitializeComponent (Handle<v8::Object> target);

	static PGconn *conn;
	static git_repository *repo;

  private:

    static NAN_METHOD(Open);
};

#endif
