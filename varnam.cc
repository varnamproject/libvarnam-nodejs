
#include <node.h>
#include <string>
#include <iostream>
#include <libvarnam/varnam.h>

using namespace v8;

const Handle<Array> perform_transliteration(varnam *handle, const char *input)
{
    int rc,i;
    varray *words;

    rc = varnam_transliterate(handle, input, &words);
    if(rc != VARNAM_SUCCESS) {
      Handle<Array> array =Array::New(1);
      array->Set(0,String::New(varnam_get_last_error(handle)));
      return array;
    }

    Handle<Array> array = Array::New(varray_length(words));

   for (i = 0; i < varray_length (words); i++)
   {
     vword *word = (vword*)varray_get (words, i);
     std::string transliterated = std::string(word->text);
     array->Set(i,String::New(transliterated.c_str()));
   }
    return array;
}

const std::string perform_reverse_transliteration(varnam *handle, const char *input)
{
    int rc;
    char *rtl;

    rc = varnam_reverse_transliterate(handle, input, &rtl);
    if(rc != VARNAM_SUCCESS) {
        return std::string(varnam_get_last_error(handle));
    }

    std::string result = std::string(rtl);
    return result;
}

const std::string perform_learn(varnam *handle, const char *input)
{
    int rc;

    rc = varnam_learn(handle, input);
    if(rc != VARNAM_SUCCESS) {
        return std::string(varnam_get_last_error(handle));
    }

    return "Success";
}

class Varnam : public node::ObjectWrap
{
public:
  static void Init(v8::Handle<v8::Object> target);
  varnam* GetHandle();
  void SetHandle(varnam *handle);
private:
  Varnam();
  ~Varnam();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value>Transliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> ReverseTransliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> Learn(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);

  varnam *handle;
};

Varnam::Varnam() {}

Varnam::~Varnam() {}

varnam* Varnam::GetHandle()
{
  return handle;
}

void Varnam::SetHandle(varnam *h)
{
  this->handle = h;
}

void Varnam::Init(Handle<Object> target)
{
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Varnam"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("learn"),
      FunctionTemplate::New(Learn)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("transliterate"),
      FunctionTemplate::New(Transliterate)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("reverseTransliterate"),
      FunctionTemplate::New(ReverseTransliterate)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("close"),
      FunctionTemplate::New(Close)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Varnam"), constructor);
}

Handle<Value> Varnam::New(const Arguments& args)
{
  char *msg;
  varnam *handle;
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value filename (args[0]->ToString());
  Varnam* obj = new Varnam();
  int rc = varnam_init(*filename, &handle, &msg);
  if (rc != VARNAM_SUCCESS) {
    ThrowException(Exception::TypeError(String::New(msg)));
    return scope.Close(Undefined());
  }

  String::Utf8Value learnings_fname (args[1]->ToString());
  rc = varnam_config (handle, VARNAM_CONFIG_ENABLE_SUGGESTIONS, *learnings_fname);
  if (rc != VARNAM_SUCCESS) {
    ThrowException(Exception::TypeError(String::New("Can't enable learnings")));
    return scope.Close(Undefined());
  }

  obj->SetHandle(handle);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> Varnam::Transliterate(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());
  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());

  Handle<Array> array =  perform_transliteration (obj->GetHandle(), *input);

  return scope.Close(array);
}

Handle<Value> Varnam::ReverseTransliterate(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());
  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  std::string transliterated = perform_reverse_transliteration (obj->GetHandle(), *input);

  return scope.Close(String::New(transliterated.c_str()));
}

Handle<Value> Varnam::Learn(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());
  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  std::string result = perform_learn (obj->GetHandle(), *input);

  return scope.Close(String::New(result.c_str()));
}

Handle<Value> Varnam::Close(const Arguments& args)
{
  HandleScope scope;

  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  varnam_destroy (obj->GetHandle());

  return scope.Close(Undefined());
}

void InitAll(Handle<Object> target)
{
  Varnam::Init (target);
}

NODE_MODULE(varnam, InitAll)
