#ifndef NT_API_H_
#define NT_API_H_

#include <v8.h>
#include <node.h>
#include <nan.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace node;

class NTApi : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> target);
        NTApi();
        ~NTApi();

    private:
        tesseract::TessBaseAPI * api;
        static NAN_METHOD(New);
        static NAN_METHOD(ocr);
        static inline Nan::Persistent<v8::Function> & constructor();
};

#endif // NT_API_H_
