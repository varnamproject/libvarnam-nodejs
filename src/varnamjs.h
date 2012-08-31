#ifndef VARNAMJS_H
#define VARNAMJS_H

#include <node.h>
#include <libvarnam/varnam.h>

class Varnam : public node::ObjectWrap
{
public:
  static void Init(v8::Handle<v8::Object> target);
  varnam* GetHandle();
  void SetHandle(varnam *handle);
private:
  Varnam() {}
  ~Varnam() {}

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value>Transliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> ReverseTransliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> Learn(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);

  varnam *handle;
};


#endif