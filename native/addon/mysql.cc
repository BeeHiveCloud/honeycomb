#include "mysql.h"

void MySQL::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(JSNewFunction);

	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(NanNew<String>("MySQL"));

	// Instance Properties
	tpl->InstanceTemplate()->SetAccessor(NanNew<String>("dummy"), GetError, SetError);

	// Prototype methods
	NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
	NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
	NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);

	// Methods
	//NODE_SET_METHOD(tpl, "dummy", Dummy);

	Local<Function> _constructor_template = tpl->GetFunction();
	NanAssignPersistent(constructor_template, _constructor_template);
	target->Set(NanNew<String>("MySQL"), tpl->GetFunction());
}

MySQL::MySQL(void *raw, bool selfFreeing){
	this->raw = raw;
	this->selfFreeing = selfFreeing;
}

MySQL::~MySQL(){

	mysql_db_disconnect(db);
}

NAN_METHOD(MySQL::JSNewFunction){
	NanScope();

	MySQL *obj = new MySQL(static_cast<void *>(Handle<External>::Cast(args[0])->Value()), args[1]->BooleanValue());
	obj->Wrap(args.Holder());

	NanReturnValue(args.Holder());
}

Handle<v8::Value> MySQL::New(void *raw, bool selfFreeing) {
	NanEscapableScope();
	Handle<v8::Value> argv[2] = { NanNew<External>((void *)raw), NanNew<Boolean>(selfFreeing) };
	return NanEscapeScope(NanNew<Function>(MySQL::constructor_template)->NewInstance(2, argv));
}

NAN_METHOD(MySQL::Open) {
    
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
    
    db = mysql_db_connect(from_mysql_host, from_mysql_user, from_mysql_passwd, from_mysql_db, from_mysql_port, NULL, 0);
    
    free((char *)from_mysql_host);
    free((char *)from_mysql_user);
    free((char *)from_mysql_passwd);
    free((char *)from_mysql_db);
    
    if(db)
        NanReturnValue(NanTrue());
    else
        NanReturnValue(NanFalse());
}

NAN_METHOD(MySQL::Set) {
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
    
    if(mysql_set_variable(db, from_name, from_value)==0)
        NanReturnValue(NanTrue());
    else
        NanReturnValue(NanFalse());
}

NAN_METHOD(MySQL::Get) {
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
    to = NanNew<String>(mysql_get_variable(db, from_name));
    
    NanReturnValue(to);
    
}

NAN_GETTER(MySQL::GetError) {
	NanScope();

	MySQL *obj = Unwrap<MySQL>(args.Holder());

	NanReturnValue(NanNew<String>(obj->error));
}

NAN_SETTER(MySQL::SetError) {
	NanScope();

	NanReturnValue(NanTrue());
}

Persistent<Function> MySQL::constructor_template;
MYSQL * MySQL::db;
