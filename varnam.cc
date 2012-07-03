#include <node.h>
#include <stdio.h>
#include <string.h>
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

char* ml_unicode_transliteration(const char *filename,const char *input)
{
    varnam *handle;
    int rc;
    char *msg;
    char *output;
    FILE *fp;   

    rc = varnam_init(filename, strlen(filename), &handle, &msg);
     if(rc != VARNAM_SUCCESS) {
        printf("initialization failed - %s\n", msg);
        return "";
    }

     rc = varnam_transliterate(handle, input, &output);
     if(rc != VARNAM_SUCCESS) {
            printf("transliteration of %s failed  - \n", input);
            return "";
      }  
    strcpy(msg,output);
    rc = varnam_destroy(handle);
 
     return msg;
}



Handle<Value> RunCallback(const Arguments& args) {
  HandleScope scope;

  Local<Function> cb = Local<Function>::Cast(args[2]);
  const unsigned argc = 1;

   v8::String::Utf8Value str1(args[0]);
   const char* cstr1 = ToCString(str1);  

   v8::String::Utf8Value str2(args[1]);
   const char* cstr2 = ToCString(str2);
 
  char*  str3 = ml_unicode_transliteration(cstr1,cstr2);

  Local<Value> argv[argc] = { Local<Value>::New(String::New(str3,strlen(str3))) };
  cb->Call(Context::GetCurrent()->Global(), argc, argv);

  return scope.Close(Undefined());
}

void Init(Handle<Object> target) {
  target->Set(String::NewSymbol("runCallback"),
      FunctionTemplate::New(RunCallback)->GetFunction());
}

NODE_MODULE(varnam, Init)
