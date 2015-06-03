#ifndef NODE_ADDON_RCONSOLE_H
#define NODE_ADDON_RCONSOLE_H

#include <nan.h>

using namespace node;
using namespace v8;

extern "C" {
  #include "../R/console.h"
}

class RConsole : public NanAsyncWorker {
 public:
   static Persistent<Function> constructor_template;
   static void InitializeComponent (Handle<v8::Object> target);

   RConsole(NanCallback *callback)
    : NanAsyncWorker(callback){}
  ~RConsole() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute ();

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback ();

 private:

   static NAN_METHOD(Run);

   static NAN_METHOD(Read);
   
};

#endif
