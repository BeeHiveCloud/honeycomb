#include "rconsole.h"

void RConsole::InitializeComponent(Handle<v8::Object> target){
  NanScope();

  Local<Object> object = NanNew<Object>();

  NODE_SET_METHOD(object, "Run", Run);
  NODE_SET_METHOD(object, "Read", Read);

  target->Set(NanNew<String>("RConsole"), object);
}

void RConsole::Execute () {
	printf("Executing \n");
	execute(0, NULL);
}

void RConsole::HandleOKCallback () {
  NanScope();

  Local<Value> argv[] = {
	  NanNull()
  };

  callback->Call(0, NULL);

}

// Asynchronous access to the `Estimate()` function
NAN_METHOD(RConsole::Run) {
  NanScope();

  NanCallback *callback = new NanCallback(args[0].As<Function>());

  NanAsyncQueueWorker(new RConsole(callback));
  NanReturnUndefined();
}

NAN_METHOD(RConsole::Read) {
	NanScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("command is required.");
	}

	// start convert_from_v8 block
	char * from_cmd;
	String::Utf8Value buf_cmd(args[0]->ToString());
	from_cmd = (char *)strdup(*buf_cmd);
	// end convert_from_v8 block

	readConsole("R", from_cmd, strlen(from_cmd), 0);

}

Persistent<Function> RConsole::constructor_template;

const char *RConsole::tprompt;
char *RConsole::tbuf;
