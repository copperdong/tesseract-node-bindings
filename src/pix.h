#ifndef NT_PIX_H_
#define NT_PIX_H_

#include <v8.h>
#include <node.h>
#include <nan.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class NTPix : public Nan::ObjectWrap {
    public: 
        NTPix();
        ~NTPix();
        static NAN_METHOD(readImage);
        static NAN_MODULE_INIT(Init);
        static Nan::Persistent<v8::Function> constructor;
        static NAN_METHOD(NewInstance);
        Pix *image;

    private:
        static NAN_METHOD(New);
        static NAN_GETTER(GetWidth);
        static NAN_GETTER(GetHeight);
        static NAN_GETTER(GetColorDepth);
};

#endif // NT_PIX_H_
