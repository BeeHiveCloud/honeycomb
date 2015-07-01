#include "mysql.h"

void Mysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "Open", Open);

	NODE_SET_METHOD(object, "Close", Close);
  
    NODE_SET_METHOD(object, "Set", Set);
    
    NODE_SET_METHOD(object, "Get", Get);

	target->Set(NanNew<String>("MySQL"), object);
}

NAN_METHOD(Mysql::Open) {
    
    NanEscapableScope();

    if (args.Length() == 0 || !args[0]->IsString()) {
        return NanThrowError("mysql host is required.");
    }
    
    if (args.Length() == 1 || !args[1]->IsString()) {
        return NanThrowError("mysql user is required.");
    }
    
    if (args.Length() == 2 || !args[2]->IsString()) {
        return NanThrowError("mysql password is required.");
    }
    
    if (args.Length() == 3 || !args[3]->IsString()) {
        return NanThrowError("mysql db is required.");
    }
    
    if (args.Length() == 4 || !args[4]->IsNumber()) {
        return NanThrowError("mysql port required.");
    }
    
    // start convert_from_v8 block
    const char * from_mysql_host;
    String::Utf8Value mysql_host(args[0]->ToString());
    from_mysql_host = (const char *) strdup(*mysql_host);
    
    const char * from_mysql_user;
    String::Utf8Value mysql_user(args[1]->ToString());
    from_mysql_user = (const char *) strdup(*mysql_user);
    
    const char * from_mysql_passwd;
    String::Utf8Value mysql_passwd(args[2]->ToString());
    from_mysql_passwd = (const char *) strdup(*mysql_passwd);
    
    const char * from_mysql_db;
    String::Utf8Value mysql_db(args[3]->ToString());
    from_mysql_db = (const char *) strdup(*mysql_db);
    
    unsigned int from_mysql_port;
    from_mysql_port = (unsigned int)args[4]->ToNumber()->Value();
    // end convert_from_v8 block
    
    db = mysql_hc_connect(from_mysql_host, from_mysql_user, from_mysql_passwd, from_mysql_db, from_mysql_port, NULL, 0);
    
    if(db)
        NanReturnValue(NanTrue());
    else
        NanReturnValue(NanFalse());
}

NAN_METHOD(Mysql::Close) {
    NanEscapableScope();
    
    mysql_hc_disconnect(db);
}

NAN_METHOD(Mysql::Set) {
    NanEscapableScope();
    
    if (args.Length() == 0 || !args[0]->IsString()) {
        return NanThrowError("name is required.");
    }
    
    if (args.Length() == 1 || !args[1]->IsString()) {
        return NanThrowError("value is required.");
    }
    
    // start convert_from_v8 block
    const char * from_name;
    String::Utf8Value name_buf(args[0]->ToString());
    from_name = (const char *) strdup(*name_buf);
    
    const char * from_value;
    String::Utf8Value value_buf(args[1]->ToString());
    from_value = (const char *) strdup(*value_buf);
    // end convert_from_v8 block
    
    if(mysql_hc_set_variable(db, from_name, from_value)==0)
        NanReturnValue(NanTrue());
    else
        NanReturnValue(NanFalse());
}

NAN_METHOD(Mysql::Get) {
    NanEscapableScope();
    
    if (args.Length() == 0 || !args[0]->IsString()) {
        return NanThrowError("name is required.");
    }
    
    // start convert_from_v8 block
    const char * from_name;
    String::Utf8Value name_buf(args[0]->ToString());
    from_name = (const char *) strdup(*name_buf);
    // end convert_from_v8 block
    
    Handle<v8::Value> to;
    to = NanNew<String>(mysql_hc_get_variable(db, from_name));
    
    NanReturnValue(to);
    
}

Persistent<Function> Mysql::constructor_template;
MYSQL * Mysql::db;
