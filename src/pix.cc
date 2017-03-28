#include <leptonica/allheaders.h>
#include <v8.h>
#include <nan.h>
#include <node.h>

#include "pix.h"

using v8::Function;
using v8::FunctionTemplate;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;

/**
 * ImgLoaderWorker - Async worker for performing ocr task
 */
class ImgLoaderWorker : public AsyncWorker {
    public:
        ImgLoaderWorker(Callback *callback, NTPix * pix_, v8::Local<v8::String> path_): AsyncWorker(callback) {
            pix = pix_;
            HandleScope scope;
            path = *Nan::Utf8String(path_);
            buffer = NULL;
            usePath = true;
            buffer_size = 0;
        }
        ImgLoaderWorker(Callback *callback, NTPix * pix_, v8::Local<v8::Object> buffer_): AsyncWorker(callback) {
            pix = pix_;
            HandleScope scope;
            buffer = (unsigned char *) node::Buffer::Data(buffer_);
            buffer_size = node::Buffer::Length(buffer_);
            usePath = false;
        }
        ~ImgLoaderWorker() {}

        void Execute () {
            if (usePath) {
                pix->image = pixRead(path.c_str());
            } else if (buffer_size > 0) {
                pix->image = pixReadMem(buffer, buffer_size);
            } else {
                pix->image = NULL;
            }
        }

        void HandleOKCallback () {
            HandleScope scope;
            Local<Value> argv[2];
            if (pix->image) {
                argv[0] = Nan::Undefined();
                argv[1] = pix->handle();
            } else {
                argv[0] = Nan::New<v8::String>("Unable to read image").ToLocalChecked();
                argv[1] = Nan::Undefined();
            }
            callback->Call(2, argv);
        }

    private:
        bool usePath;
        unsigned char * buffer;
        size_t buffer_size;
        std::string path;
        NTPix * pix;
};

NTPix::NTPix() {
    image = NULL;
}

NTPix::~NTPix() {
    pixDestroy(&image);
}

Nan::Persistent<Function> NTPix::constructor;

       
NAN_MODULE_INIT(NTPix::Init) {
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Pix").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Attach method to base module.
    Nan::SetMethod(target, "readImage", NTPix::readImage);

    constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(NTPix::NewInstance) {
    Local<Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(Nan::NewInstance(cons, 0, NULL).ToLocalChecked());
}

NAN_METHOD(NTPix::New) {
    NTPix * obj = new NTPix();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(NTPix::readImage) {
    if (info.Length() != 2) {
        Nan::ThrowTypeError("readImage requries two arguments: (string|buffer, callback)");
        return;
    }
    Callback *callback = new Callback(info[1].As<Function>());

    Local<Function> cons = Nan::New(constructor);
    Local<v8::Object> obj = Nan::NewInstance(cons, 0, NULL).ToLocalChecked();
    NTPix * pix = Nan::ObjectWrap::Unwrap<NTPix>(obj);

    if (info[0]->IsString()) {
        Local<v8::String> path = To<v8::String>(info[0]).ToLocalChecked();
        AsyncQueueWorker(new ImgLoaderWorker(callback, pix, path));
    } else if (node::Buffer::HasInstance(info[0])) {
        Local<v8::Object> buffer = To<v8::Object>(info[0]).ToLocalChecked();
        AsyncQueueWorker(new ImgLoaderWorker(callback, pix, buffer));
    } else {
        Nan::ThrowTypeError("readImage requries two arguments: (string|buffer, callback)");
    }
}
