#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <lwes.h> 

using namespace node;
using namespace v8;

class LwesParser {
public:
  static Persistent<FunctionTemplate> ft;
  
  static void Init(Handle<Object> target) {
    // Create a local scope.
    // All js objects will be gc'ed at when the ecope exits
    // if not decleared persistent.
    // The scope exits when the scope is released.
    // In this case, when this function exits, the scope is released.
    HandleScope scope;
    
    // create a new function template (js function)
    // This functions wraps the Parse function
    Local<FunctionTemplate> local = FunctionTemplate::New(Parse);
    
    // declear this function template persistent.
    // so javascript doesn't scrape it when exiting this function.
    LwesParser::ft = Persistent<FunctionTemplate>::New(local);

    // target is 'exports' in this case.
    // Key 'parse' is mapped to Parse function that we created.
    target->Set(String::NewSymbol("parse"), LwesParser::ft->GetFunction());
  }
  
  static Handle<Value> Parse(const Arguments& args) {
    HandleScope scope;
    
    // check the first arg is a buffer
    if (!Buffer::HasInstance(args[0])) {
      return ThrowException(Exception::TypeError(String::New("Argument should be a buffer")));
    }
    
    // get node buffer js object, and store in local handle
    Local<Object> buffer = args[0]->ToObject();
    
    // get memory pointer to raw buffer data
    LWES_BYTE_P bytes = (LWES_BYTE_P)Buffer::Data(buffer);
    
    // get buffer length in bytes
    size_t num_bytes = (size_t)Buffer::Length(buffer);
    
    // init some lwes structs
    struct lwes_event *e;
    struct lwes_event_deserialize_tmp dtmp;
    int rc;
    e = lwes_event_create_no_name(NULL);
    
    // the actual parse
    rc = lwes_event_from_bytes(e, bytes, num_bytes, 0, &dtmp);
    
    // if rc is less than 0, then there is a parse error
    if (rc < 0) {
      // release event memory
      lwes_event_destroy(e);
      
      // return with exceptions
      char message[256];
      snprintf(message,
               sizeof(message),
               "failed to parse LWES event (code: %d)",
               rc);
      return ThrowException(Exception::TypeError(String::New(message)));
    }
    
    // event parse succeeded.
    // extract this event into a js object
    Handle<Object> event = WrapLwesParser(e);
    
    // release memory
    lwes_event_destroy(e);
    
    // return the event
    return event;
  }
  
  // convert lwes_event struct to js object here
  static Handle<Object> WrapLwesParser(const struct lwes_event *e) {
    HandleScope scope;
    Local<Object> event = Object::New();
    
    // do the conversion here...
    
    return event;
  }
  
};

Persistent<FunctionTemplate> LwesParser::ft;

extern "C" {
  static void init(Handle<Object> target) {
    LwesParser::Init(target);
  }
  
  NODE_MODULE(LwesParser, init);
}