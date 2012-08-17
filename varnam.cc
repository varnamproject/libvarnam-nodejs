#include <node.h>
#include <stdio.h>
#include <string>
#define LINE_MAX 1000

extern "C" {
   #include <libvarnam/varnam.h>
}
using namespace std;
using namespace v8;


// Extracts a C string from a V8 Utf8Value.
const char* ToCString(v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

const std::string ml_unicode_transliteration(char *filename,char *input)
{
    varnam *handle;
    int rc;
    char *msg;
    varray *words;

    rc = varnam_init(filename, &handle, &msg);
     if(rc != VARNAM_SUCCESS) {
           printf ("Initialization failed. %s\n", msg);
        return "";
    }

     rc = varnam_transliterate(handle, input, &words);
      if(rc != VARNAM_SUCCESS) {
         printf ("Transliteration failed. %s\n", varnam_get_last_error(handle));
        return "";
      }

    vword *word = (vword*) varray_get(words, 0);
    std::string result = std::string(word->text);
    varnam_destroy(handle);
    return result;
}



Handle<Value> RunCallback(const Arguments& args) {
  HandleScope scope;

  Local<Function> cb = Local<Function>::Cast(args[2]);
  const unsigned argc = 1;

   v8::String::Utf8Value str1(args[0]);
 //  const char* cstr1 = ToCString(str1);
  char* cstr1 = str1.operator*();

  v8::String::Utf8Value str2(args[1]);
  char* cstr2 = str2.operator*();

  const std::string result = ml_unicode_transliteration(cstr1,cstr2);

  Local<Value> argv[argc] = { Local<Value>::New(String::New(result.c_str())) };
  cb->Call(Context::GetCurrent()->Global(), argc, argv);

  return scope.Close(Undefined());
}

void Init(Handle<Object> target) {
  target->Set(String::NewSymbol("runCallback"),
      FunctionTemplate::New(RunCallback)->GetFunction());
}

NODE_MODULE(varnam, Init)
