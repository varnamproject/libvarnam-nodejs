#ifndef VARNAMJS_H
#define VARNAMJS_H

#include <queue>
#include <node.h>
#include <libvarnam/varnam.h>

class Varnam : public node::ObjectWrap
{
public:
  static void Init(v8::Handle<v8::Object> target);
  bool InitializeFirstHandle(std::string& error);
  varnam* GetHandle();

private:
  Varnam(const char* scheme, const char* learnings)
  	: scheme_file(scheme), learnings_file(learnings)
  {

  }
  ~Varnam() {}

  bool CreateNewVarnamHandle(varnam** handle, std::string& error);

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value>Transliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> ReverseTransliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> Learn(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);

  const std::string scheme_file;
  const std::string learnings_file;
  std::queue<varnam*> handles;
};

#endif