
#include <node.h>
#include <string>
#include <libvarnam/varnam.h>

using namespace v8;

const std::string perform_transliteration(const char *filename, const char *input)
{
    varnam *handle;
    int rc;
    char *msg;
    varray *words;

    rc = varnam_init(filename, &handle, &msg);
     if(rc != VARNAM_SUCCESS) {
        return std::string(msg);
    }

     rc = varnam_transliterate(handle, input, &words);
      if(rc != VARNAM_SUCCESS) {
         return std::string(msg);
      }

    vword *word = (vword*) varray_get(words, 0);
    std::string result = std::string(word->text);
    varnam_destroy(handle);
    return result;
}

const std::string perform_reverse_transliteration(const char *filename, const char *input)
{
    varnam *handle;
    int rc;
    char *msg;
    char *rtl;

    rc = varnam_init(filename, &handle, &msg);
     if(rc != VARNAM_SUCCESS) {
        return std::string(msg);
    }

    rc = varnam_reverse_transliterate(handle, input, &rtl);
    if(rc != VARNAM_SUCCESS) {
        return std::string(msg);
    }

    std::string result = std::string(rtl);
    varnam_destroy(handle);
    return result;
}

Handle<Value> Transliterate(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value filename (args[0]->ToString());
  String::Utf8Value input (args[1]->ToString());

  std::string transliterated = perform_transliteration (*filename, *input);
  return scope.Close(String::New(transliterated.c_str()));
}

Handle<Value> ReverseTransliterate(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value filename (args[0]->ToString());
  String::Utf8Value input (args[1]->ToString());

  std::string transliterated = perform_reverse_transliteration (*filename, *input);
  return scope.Close(String::New(transliterated.c_str()));
}

void Init(Handle<Object> target)
{
  target->Set(String::NewSymbol("transliterate"), FunctionTemplate::New(Transliterate)->GetFunction());
  target->Set(String::NewSymbol("reverse_transliterate"), FunctionTemplate::New(ReverseTransliterate)->GetFunction());
}

NODE_MODULE(varnam, Init)
