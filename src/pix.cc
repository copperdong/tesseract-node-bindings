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
 * EstimateSizeOfPix
 */
int estimateSizeOfPix(Pix * p) {
    return p->w * p->h * p->d;
}


/**
 * ImgLoaderWorker - Async worker for performing ocr task
 */
class ImgLoaderWorker : public AsyncWorker {
    public:
        ImgLoaderWorker(Callback *callback, v8::Local<v8::String> path_): AsyncWorker(callback) {
            HandleScope scope;
            path = *Nan::Utf8String(path_);
            buffer = NULL;
            usePath = true;
            buffer_size = 0;
        }
        ImgLoaderWorker(Callback *callback, v8::Local<v8::Object> buffer_): AsyncWorker(callback) {
            HandleScope scope;
            buffer = (unsigned char *) node::Buffer::Data(buffer_);
            buffer_size = node::Buffer::Length(buffer_);
            usePath = false;
        }
        ~ImgLoaderWorker() {}

        void Execute () {
            if (usePath) {
                image = pixRead(path.c_str());
            } else if (buffer_size > 0) {
                image = pixReadMem(buffer, buffer_size);
            } else {
                image = NULL;
            }
            if (!image) {
                SetErrorMessage("The given image was not readable");
            }
        }

        void HandleOKCallback () {
            HandleScope scope;

            Local<Function> cons = Nan::New(NTPix::constructor);
            Local<v8::Object> obj = Nan::NewInstance(cons, 0, NULL).ToLocalChecked();
            NTPix * pix = Nan::ObjectWrap::Unwrap<NTPix>(obj);
            pix->image = image;
            Nan::AdjustExternalMemory(estimateSizeOfPix(image));

            Local<Value> argv[2];
            argv[0] = Nan::Undefined();
            argv[1] = pix->handle();
            callback->Call(2, argv);
        }

    private:
        bool usePath;
        unsigned char * buffer;
        size_t buffer_size;
        std::string path;
        Pix * image;
};

NTPix::NTPix() {
    image = NULL;
}

NTPix::~NTPix() {
    if (image) {
        pixDestroy(&image);
        Nan::AdjustExternalMemory(-estimateSizeOfPix(image));
    }
}

Nan::Persistent<Function> NTPix::constructor;

       
NAN_MODULE_INIT(NTPix::Init) {
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Pix").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();
    proto->SetInternalFieldCount(1);

    // Attach method to base module.
    Nan::SetMethod(target, "readImage", NTPix::readImage);

    // Accessors
    v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl, Nan::New("height").ToLocalChecked(), GetHeight);
    Nan::SetAccessor(itpl, Nan::New("width").ToLocalChecked(), GetWidth);
    Nan::SetAccessor(itpl, Nan::New("depth").ToLocalChecked(), GetColorDepth);

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

    if (info[0]->IsString()) {
        Local<v8::String> path = To<v8::String>(info[0]).ToLocalChecked();
        AsyncQueueWorker(new ImgLoaderWorker(callback, path));
    } else if (node::Buffer::HasInstance(info[0])) {
        Local<v8::Object> buffer = To<v8::Object>(info[0]).ToLocalChecked();
        AsyncQueueWorker(new ImgLoaderWorker(callback, buffer));
    } else {
        Nan::ThrowTypeError("readImage requries two arguments: (string|buffer, callback)");
    }
}

NAN_GETTER(NTPix::GetWidth) {
    NTPix *obj = ObjectWrap::Unwrap<NTPix>(info.This());
    info.GetReturnValue().Set(Nan::New<v8::Number>(obj->image->w));
}

NAN_GETTER(NTPix::GetHeight) {
    NTPix *obj = ObjectWrap::Unwrap<NTPix>(info.This());
    info.GetReturnValue().Set(Nan::New<v8::Number>(obj->image->h));
}

NAN_GETTER(NTPix::GetColorDepth) {
    NTPix *obj = ObjectWrap::Unwrap<NTPix>(info.This());
    info.GetReturnValue().Set(Nan::New<v8::Number>(obj->image->d));
}
