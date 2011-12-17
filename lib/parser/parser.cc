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
               "failed to parse LWES event (coded %d)",
               rc);
      return ThrowException(Exception::TypeError(String::New(message)));
    }

    // event parse succeeded.
    // extract this event into a js object
    Handle<Value> event = WrapLwesParser(e);

    // release memory
    lwes_event_destroy(e);

    // return the event
    return event;
  }

  static Handle<Object> GetUInt16(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_U_INT_16 value;
    lwes_event_get_U_INT_16(e, key, &value);

    attribute->Set(String::New("type"), String::New("UINT16"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetInt16(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_INT_16 value;
    lwes_event_get_INT_16(e, key, &value);

    attribute->Set(String::New("type"), String::New("INT16"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetUInt32(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_U_INT_32 value;
    lwes_event_get_U_INT_32(e, key, &value);

    attribute->Set(String::New("type"), String::New("UINT32"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetInt32(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_INT_32 value;
    lwes_event_get_INT_32(e, key, &value);

    attribute->Set(String::New("type"), String::New("INT32"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetUInt64(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_U_INT_64 value;
    lwes_event_get_U_INT_64(e, key, &value);

    attribute->Set(String::New("type"), String::New("UINT64"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetInt64(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_INT_64 value;
    lwes_event_get_INT_64(e, key, &value);

    attribute->Set(String::New("type"), String::New("INT64"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  static Handle<Object> GetString(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_LONG_STRING value;
    lwes_event_get_STRING(e, key, &value);

    attribute->Set(String::New("type"), String::New("STRING"));
    attribute->Set(String::New("value"), String::New(value));
    return attribute;
  }

  static Handle<Object> GetIpAddr(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_IP_ADDR value;
    lwes_event_get_IP_ADDR(e, key, &value);

    attribute->Set(String::New("type"), String::New("IPADDR"));
    attribute->Set(String::New("value"), String::New(inet_ntoa(value)));
    return attribute;
  }

  static Handle<Object> GetBoolean(struct lwes_event *e, LWES_CONST_SHORT_STRING key) {
    Local<Object> attribute = Object::New();
    LWES_BOOLEAN value;
    lwes_event_get_BOOLEAN(e, key, &value);

    attribute->Set(String::New("type"), String::New("BOOLEAN"));
    attribute->Set(String::New("value"), Number::New(value));
    return attribute;
  }

  // convert lwes_event struct to js object here
  static Handle<Value> WrapLwesParser(struct lwes_event *e) {
    HandleScope scope;
    Local<Object> event = Object::New();

    LWES_SHORT_STRING event_name;
    lwes_event_get_name(e, &event_name);

    event->Set(String::New("type"), String::New(event_name));

    Local<Object> attributes = Object::New();
    event->Set(String::New("attributes"), attributes);

    // enumerate event attributes
    struct lwes_event_enumeration enumeration;
    if(lwes_event_keys(e, &enumeration)) {
      LWES_CONST_SHORT_STRING key;
      LWES_TYPE type;
      while (lwes_event_enumeration_next_element(&enumeration, &key, &type)) {
        switch (type) {
          case LWES_TYPE_U_INT_16:
            attributes->Set(String::New(key), GetUInt16(e, key));
            break;

          case LWES_TYPE_INT_16:
            attributes->Set(String::New(key), GetInt16(e, key));
            break;

          case LWES_TYPE_U_INT_32:
            attributes->Set(String::New(key), GetUInt32(e, key));
            break;

          case LWES_TYPE_INT_32:
            attributes->Set(String::New(key), GetInt32(e, key));
            break;

          case LWES_TYPE_U_INT_64:
            attributes->Set(String::New(key), GetUInt64(e, key));
            break;

          case LWES_TYPE_INT_64:
            attributes->Set(String::New(key), GetInt64(e, key));
            break;

          case LWES_TYPE_STRING:
            attributes->Set(String::New(key), GetString(e, key));
            break;

          case LWES_TYPE_IP_ADDR:
            attributes->Set(String::New(key), GetIpAddr(e, key));
            break;


          case LWES_TYPE_BOOLEAN:
            attributes->Set(String::New(key), GetBoolean(e, key));
            break;

          default:
            char message[1024];
            snprintf( message,
                    sizeof(message),
                    "failed to parse LWES attribute (key %s)",
                    key );

            return ThrowException(Exception::Error(String::New(message)));
            break;
        }
      }
    }
    return event;
  }
};

Persistent<FunctionTemplate> LwesParser::ft;

extern "C" {
  static void init(Handle<Object> target) {
    LwesParser::Init(target);
  }

  NODE_MODULE(parser, init);
}
