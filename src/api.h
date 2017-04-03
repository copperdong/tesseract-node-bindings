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
        static NAN_METHOD(ocr);
};

#endif // NT_API_H_
