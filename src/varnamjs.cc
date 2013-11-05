#include <string>
#include <iostream>

#include "varnamjs.h"

using namespace v8;

#ifdef _WIN32
    #include <windows.h>

    void vsleep(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }
#else
    #include <unistd.h>

    void vsleep(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // takes microseconds
    }
#endif

void perform_transliteration_async(uv_work_t *req)
{
    WorkerData *data = static_cast<WorkerData*>(req->data);

    int tries = 0;
    int maxtries = 10;
    varnam* handle = NULL;
    while (++tries < maxtries)
    {
      handle = data->clazz->GetHandle();
      if (handle != NULL)
        break;
      vsleep (10);
    }

    if (handle == NULL)
    {
      // We couldn't acquire a handle after 10 tries. This is unusual.
      data->errored = true;
      data->error_message = "Couldn't acquire a varnam handle";
      return;
    }

    varray *words;

    int rc = varnam_transliterate(handle, data->text_to_tl.c_str(), &words);
    if(rc != VARNAM_SUCCESS) {
      data->errored = true;
      data->error_message = varnam_get_last_error(handle);
    }
    else {
      for (int i = 0; i < varray_length (words); i++)
      {
        vword *word = (vword*) varray_get (words, i);
        data->tl_output.push_back (std::string(word->text));
      }
    }

    data->clazz->ReturnHandle(handle);
}

// libuv invokes this in main thread. Safe to access V8 objects
void after_transliteration(uv_work_t *req)
{
    WorkerData *data = static_cast<WorkerData*>(req->data);
    if (data->errored)
    {
      Local<Object> error = Object::New();
      error->Set(String::New("message"), String::New(data->error_message.c_str()));
      Handle<Value> argv[] = { error, Null() };
      data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    }
    else
    {
      Handle<Array> array = Array::New(data->tl_output.size());
      std::vector<std::string>& result = data->tl_output;
      int i = 0;
      for(std::vector<std::string>::const_iterator it = result.begin(); it < result.end(); it++)
      {
        array->Set(i++, String::New((*it).c_str()));
      }

      Handle<Value> argv[] = { Null(), array };
      data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    }

    data->clazz->Unref();
    data->callback.Dispose();
    delete data;
    data = NULL;
}

void perform_learn_async(uv_work_t *req)
{
    WorkerData *data = static_cast<WorkerData*>(req->data);
    Varnam* clazz = data->clazz;

    uv_mutex_lock (clazz->GetMutexForLearn());
    data->errored = false;
    varnam* handle = clazz->GetHandleForLearn();
    int rc = varnam_learn (handle, data->word_to_learn.c_str());
    if (rc != VARNAM_SUCCESS) {
      data->errored = true;
      data->error_message = varnam_get_last_error(handle);
    }
    uv_mutex_unlock (clazz->GetMutexForLearn());
}

void after_learn(uv_work_t* req)
{
    WorkerData *data = static_cast<WorkerData*>(req->data);
    if (data->errored)
    {
      Local<Object> error = Object::New();
      error->Set(String::New("message"), String::New(data->error_message.c_str()));
      Handle<Value> argv[] = { error };
      data->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    }
    else
    {
      Handle<Value> argv[] = { Null() };
      data->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    }

    data->clazz->Unref();
    data->callback.Dispose();
    delete data;
    data = NULL;
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

// Each worker thread will use this function to get a varnam handle.
// If no free handles are available, and we reached the maximum limit of handles,
// this function returns null. Caller can call this function again after sometime.
varnam* Varnam::GetHandle()
{
  varnam* handle = NULL;

  uv_mutex_lock (&mutex);
  if (!handles_available.empty())
  {
    handle = handles_available.front();
  }
  else
  {
    std::string error;
    // TODO: Handle errors
    CreateNewVarnamHandle(&handle, error);
    assert(handle->internal);
    handles.push_back (handle);
    handles_available.push (handle);
  }

  handles_available.pop();
  uv_mutex_unlock (&mutex);
  return handle;
}

void Varnam::ReturnHandle(varnam* handle)
{
  uv_mutex_lock (&mutex);
  handles_available.push (handle);
  uv_mutex_unlock (&mutex);
}

bool Varnam::InitializeFirstHandle(std::string& error)
{
  varnam* handle;
  bool created = CreateNewVarnamHandle(&handle, error);
  if (created)
  {
    handles.push_back (handle);
    handles_available.push (handle);
  }

  // Initialize handle to learn
  varnam* handle2;
  created = CreateNewVarnamHandle(&handle2, error);
  SetHandleToLearn(handle2);
  return created;
}

int Varnam::GetHandleCount()
{
  return handles.size();
}

bool Varnam::CreateNewVarnamHandle(varnam** handle, std::string& error)
{
  char *msg;
  int rc = varnam_init(scheme_file.c_str(), handle, &msg);
  if (rc != VARNAM_SUCCESS) {
    error += msg;
    return false;
  }

  rc = varnam_config (*handle, VARNAM_CONFIG_ENABLE_SUGGESTIONS, learnings_file.c_str());
  if (rc != VARNAM_SUCCESS) {
    error += varnam_get_last_error (*handle);
    return false;
  }

  return true;
}

void Varnam::Init(Handle<Object> target)
{
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Varnam"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("learn"),
      FunctionTemplate::New(Learn)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("isKnownWord"),
      FunctionTemplate::New(IsKnownWord)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("transliterate"),
      FunctionTemplate::New(Transliterate)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("transliterateSync"),
      FunctionTemplate::New(TransliterateSync)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("reverseTransliterate"),
      FunctionTemplate::New(ReverseTransliterate)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("close"),
      FunctionTemplate::New(Close)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("getOpenHandles"),
      FunctionTemplate::New(GetOpenHandles)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Varnam"), constructor);
}

Handle<Value> Varnam::New(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value filename (args[0]->ToString());
  String::Utf8Value learnings_fname (args[1]->ToString());

  Varnam* obj = new Varnam(*filename, *learnings_fname);
  std::string error;
  bool initialized = obj->InitializeFirstHandle(error);
  if (!initialized) {
    ThrowException(Exception::TypeError(String::New(error.c_str())));
    return scope.Close(Undefined());
  }

  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> Varnam::Transliterate(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("First argument should be string")));
    return scope.Close(Undefined());
  }

  if (!args[1]->IsFunction()) {
    ThrowException(Exception::TypeError(String::New("Second argument should be a function")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());

  WorkerData* data = new WorkerData;
  data->errored = false;
  data->tl_output = std::vector<std::string>();
  data->error_message = "";
  data->request.data = data;
  data->text_to_tl = *input;
  data->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  data->clazz = ObjectWrap::Unwrap<Varnam>(args.This());
  data->clazz->Ref();

  uv_queue_work(uv_default_loop(), &data->request, perform_transliteration_async, (uv_after_work_cb) after_transliteration);

  return scope.Close(Undefined());
}

Handle<Value> Varnam::TransliterateSync(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("First argument should be string")));
    return scope.Close(Undefined());
  }

  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  String::Utf8Value input (args[0]->ToString());

  varray *words;
  varnam *handle = obj->GetHandle();
  int rc = varnam_transliterate(handle, *input, &words);
  if(rc != VARNAM_SUCCESS) {
    ThrowException(Exception::TypeError(String::New(varnam_get_last_error(handle))));
    obj->ReturnHandle(handle);
    return scope.Close(Undefined());
  }

  Handle<Array> array = Array::New(varray_length (words));
  for (int i = 0; i < varray_length (words); i++)
  {
    vword *word = (vword*) varray_get (words, i);
    array->Set(i, String::New(word->text));
  }

  obj->ReturnHandle(handle);
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

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("First argument should be string")));
    return scope.Close(Undefined());
  }

  if (!args[1]->IsFunction()) {
    ThrowException(Exception::TypeError(String::New("Second argument should be a function")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());

  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  obj->Ref();

  WorkerData* data = new WorkerData;
  data->errored = false;
  data->request.data = data;
  data->error_message = "";
  data->word_to_learn = *input;
  data->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  data->clazz = obj;

  uv_queue_work (uv_default_loop(), &data->request, perform_learn_async, (uv_after_work_cb) after_learn);

  return scope.Close(Undefined());
}

Handle<Value> Varnam::IsKnownWord(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("First argument should be string")));
    return scope.Close(Undefined());
  }

  String::Utf8Value input (args[0]->ToString());

  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  bool known = varnam_is_known_word (obj->GetHandle(), *input);

  return scope.Close(Boolean::New(known));
}
Handle<Value> Varnam::Close(const Arguments& args)
{
  HandleScope scope;

  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  obj->Dispose();

  return scope.Close(Undefined());
}

Handle<Value> Varnam::GetOpenHandles(const Arguments& args)
{
  HandleScope scope;
  Varnam* obj = ObjectWrap::Unwrap<Varnam>(args.This());
  return scope.Close(Number::New(obj->GetHandleCount()));
}

void Varnam::Dispose()
{
  std::vector<varnam*>::iterator it;
  for (it = handles.begin(); it < handles.end(); it++)
  {
    varnam_destroy (*it);
  }
}

void InitAll(Handle<Object> target)
{
  Varnam::Init (target);
}

NODE_MODULE(varnam, InitAll)
