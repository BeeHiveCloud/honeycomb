#include "git_mysql.h"

GitMysql::GitMysql() {
   git_mysql wrappedValue;
   this->raw = (git_mysql *)malloc(sizeof(git_mysql));
   memcpy(this->raw, &wrappedValue, sizeof(git_mysql));

  this->ConstructFields();
  this->selfFreeing = true;
}

GitMysql::GitMysql(git_mysql *raw, bool selfFreeing) {
  this->raw = raw;
  this->ConstructFields();
  this->selfFreeing = selfFreeing;
}

GitMysql::~GitMysql() {
  if (this->selfFreeing) {
    free(this->raw);
  }
}

void GitMysql::ConstructFields() {}

void GitMysql::InitializeComponent(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("GitMysql"));

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Mysql"), _constructor_template);
}

NAN_METHOD(GitMysql::JSNewFunction) {
  NanScope();
  GitMysql* instance;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    instance = new GitMysql();
  }
  else {
    instance = new GitMysql(static_cast<git_mysql*>(Handle<External>::Cast(args[0])->Value()), args[1]->BooleanValue());
  }

  instance->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<v8::Value> GitMysql::New(void* raw, bool selfFreeing) {
  NanEscapableScope();

  Handle<v8::Value> argv[2] = { NanNew<External>((void *)raw), NanNew<Boolean>(selfFreeing) };
  return NanEscapeScope(NanNew<Function>(GitMysql::constructor_template)->NewInstance(2, argv));
}

git_mysql *GitMysql::GetValue() {
  return this->raw;
}

git_mysql **GitMysql::GetRefValue() {
  return this->raw == NULL ? NULL : &this->raw;
}

void GitMysql::ClearValue() {
  this->raw = NULL;
}

Persistent<Function> GitMysql::constructor_template;
