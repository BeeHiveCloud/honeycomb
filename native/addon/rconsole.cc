#include "rconsole.h"

void RConsole::InitializeComponent(Handle<v8::Object> target){
  NanScope();

  Local<Object> object = NanNew<Object>();

  NODE_SET_METHOD(object, "Run", Run);

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

Persistent<Function> RConsole::constructor_template;
