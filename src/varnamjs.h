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
  void ReturnHandle(varnam* handle);
  void Dispose();
  int GetHandleCount();

private:
  Varnam(const char* scheme, const char* learnings)
  	: scheme_file(scheme), learnings_file(learnings)
  {
  	uv_mutex_init (&mutex);
  }
  ~Varnam() {}

  bool CreateNewVarnamHandle(varnam** handle, std::string& error);

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Transliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> ReverseTransliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> Learn(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetOpenHandles(const v8::Arguments& args);

  const std::string scheme_file;
  const std::string learnings_file;

  std::vector<varnam*> handles;           // Contains reference to all handles
  std::queue<varnam*> handles_available;  // Contains handles which are ready to do work
  uv_mutex_t mutex;
};

struct WorkerData
{
	uv_work_t request;
	Varnam* clazz;

	// For transliteration
	std::string text_to_tl;
	std::vector<std::string> tl_output;

	bool errored;
	std::string error_message;

	v8::Persistent<v8::Function> callback;
};


#endif