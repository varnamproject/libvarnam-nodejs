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

  uv_mutex_t* GetMutexForLearn()
  {
      return &learn_mutex;
  }

  varnam* GetHandleForLearn()
  {
      return learn_handle;
  }

  void SetHandleToLearn(varnam* h)
  {
      learn_handle = h;
  }

  void Ref() { node::ObjectWrap::Ref(); }
  void Unref() { node::ObjectWrap::Unref(); }

private:
  Varnam(const char* scheme, const char* learnings)
  	: scheme_file(scheme), learnings_file(learnings)
  {
      uv_mutex_init (&mutex);
      uv_mutex_init (&learn_mutex);
  }
  ~Varnam()
  {
      uv_mutex_destroy (&mutex);
      uv_mutex_destroy (&learn_mutex);
  }

  bool CreateNewVarnamHandle(varnam** handle, std::string& error);

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Transliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> TransliterateSync(const v8::Arguments& args);
  static v8::Handle<v8::Value> ReverseTransliterate(const v8::Arguments& args);
  static v8::Handle<v8::Value> Learn(const v8::Arguments& args);
  static v8::Handle<v8::Value> IsKnownWord(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetOpenHandles(const v8::Arguments& args);

  const std::string scheme_file;
  const std::string learnings_file;

  varnam* learn_handle;
  std::vector<varnam*> handles;           // Contains reference to all handles
  std::queue<varnam*> handles_available;  // Contains handles which are ready to do work
  uv_mutex_t mutex;
  uv_mutex_t learn_mutex;
};

struct WorkerData
{
	uv_work_t request;
	Varnam* clazz;

	// For transliteration
	std::string text_to_tl;
	std::vector<std::string> tl_output;

  std::string word_to_learn;

	bool errored;
	std::string error_message;

	v8::Persistent<v8::Function> callback;
};


#endif
