#include <v8.h>
#include <node.h>
#include <nan.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "api.h"
#include "pix.h"

using v8::Function;
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
using Nan::Get;
using tesseract::TessBaseAPI;

class RectBox {
    public:
        uint32_t x, y, w, h;
};

/**
 * OCRWorker - Async worker for performing ocr task
 */
class OCRWorker : public AsyncWorker {
    public:
        OCRWorker(Callback *callback, NTPix * image, TessBaseAPI * api, v8::Local<v8::Object> options)
            : AsyncWorker(callback), image(image), api(api), text(NULL) {

            Local<v8::String> psm_key = Nan::New("psm").ToLocalChecked();
            Local<v8::String> rect_key = Nan::New("rect").ToLocalChecked();
            
            Local<v8::Value> local_psm = Get(options, psm_key).ToLocalChecked();
            Local<v8::Value> local_rect = Get(options, rect_key).ToLocalChecked();
            
            // Defaults
            psm = tesseract::PSM_SINGLE_BLOCK;
            rect.w = 0;
            rect.h = 0;
            rect.x = 0;
            rect.y = 0;

            if (!local_psm->IsUndefined()) {
                if(local_psm->IsNumber()) {
                    psm = (tesseract::PageSegMode) To<int>(local_psm).FromJust();
                } else {
                    SetErrorMessage("OCR option psm must be an integer");
                    return;
                }
            }

            if (!local_rect->IsUndefined()) {
                if (local_rect->IsArray()) {
                    Local<v8::Array> rect_arr = local_rect.As<v8::Array>();
                    Local<v8::Value> local_rect_0 = Get(rect_arr, 0).ToLocalChecked();
                    Local<v8::Value> local_rect_1 = Get(rect_arr, 1).ToLocalChecked();
                    Local<v8::Value> local_rect_2 = Get(rect_arr, 2).ToLocalChecked();
                    Local<v8::Value> local_rect_3 = Get(rect_arr, 3).ToLocalChecked();
                    bool all_ints =
                        local_rect_0->IsNumber() &&
                        local_rect_1->IsNumber() &&
                        local_rect_2->IsNumber() &&
                        local_rect_3->IsNumber();
                    if (rect_arr->Length() == 4 && all_ints) {
                        rect.x = To<uint32_t>(local_rect_0).FromJust();
                        rect.y = To<uint32_t>(local_rect_1).FromJust();
                        rect.w = To<uint32_t>(local_rect_2).FromJust();
                        rect.h = To<uint32_t>(local_rect_3).FromJust();
                    } else {
                        SetErrorMessage("OCR option rect must be an array of four integers ");
                        return;
                    }
                } else {
                    SetErrorMessage("OCR option rect must be an array of four integers ");
                    return;
                }
            }
        }

        ~OCRWorker() {}

        void Execute () {
            if (ErrorMessage()) {
                return;
            }
            api->SetPageSegMode(psm);
            api->SetImage(image->image);
            if (rect.w > 0 && rect.h > 0) {
                api->SetRectangle(rect.x, rect.y, rect.w, rect.h);
            }
            text = api->GetUTF8Text();
            api->Clear();
        }

        void HandleOKCallback() {
          HandleScope scope;

          Local<Value> argv[] = { Null(), Nan::New<v8::String>(text).ToLocalChecked() };
          callback->Call(2, argv);
        }

    private:
        NTPix * image;
        TessBaseAPI * api;
        char * text;
        v8::Local<v8::Object> options;
        tesseract::PageSegMode psm;
        RectBox rect;
};

NAN_MODULE_INIT(NTApi::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(NTApi::New);
    tpl->SetClassName(Nan::New("BaseAPI").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    std::string version = tesseract::TessBaseAPI::Version();

    tpl->Set(Nan::New<v8::String>("version").ToLocalChecked(), Nan::New<v8::String>(version).ToLocalChecked());
    tpl->PrototypeTemplate()->Set(Nan::New<v8::String>("version").ToLocalChecked(), Nan::New<v8::String>(version).ToLocalChecked());
    Nan::SetPrototypeMethod(tpl, "ocr", NTApi::ocr);

    constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target, Nan::New("BaseAPI").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}


NTApi::NTApi() {
    api = new tesseract::TessBaseAPI();
}

NTApi::~NTApi() {
    api->End();
}

NAN_METHOD(NTApi::New) {
    if (info.IsConstructCall()) {
        char const * datapath = NULL;
        std::string lang("eng");
        tesseract::OcrEngineMode mode = tesseract::OEM_DEFAULT;
        std::vector<char *> configs;

        GenericVector<STRING> *vars_vec = NULL;
        GenericVector<STRING> *vars_values = NULL;
        bool set_only_non_debug_params = true;

        if (info.Length() == 0) {
            // Just use the defaults
        } else if (info.Length() == 1 && info[0]->IsObject()) {
            Nan::MaybeLocal<v8::Object> maybeOptions = Nan::To<v8::Object>(info[0]);
            if (!maybeOptions.IsEmpty()) {
                v8::Local<v8::Object> options = maybeOptions.ToLocalChecked();

                Nan::MaybeLocal<v8::Value> maybeLang = Nan::Get(options, Nan::New<v8::String>("lang").ToLocalChecked());
                if (!maybeLang.IsEmpty()) {
                    Local<v8::Value> vlang = maybeLang.ToLocalChecked();
                    if (vlang->IsString()) {
                        lang = std::string(*Nan::Utf8String(maybeLang.ToLocalChecked()));
                    }
                }

                Nan::MaybeLocal<v8::Value> maybeDataPath = Nan::Get(options, Nan::New<v8::String>("datapath").ToLocalChecked());
                if (!maybeDataPath.IsEmpty()) {
                    Local<v8::Value> vdatapath = maybeDataPath.ToLocalChecked();
                    if (vdatapath->IsString()) {
                        datapath = *Nan::Utf8String(maybeDataPath.ToLocalChecked());
                    }
                }
            }
        } else {
            Nan::ThrowTypeError("Expected zero or one arguments (options object).");
            return;
        }

        NTApi * obj = new NTApi();
        
        if (obj->api->Init(datapath, lang.c_str(), mode, &configs[0], configs.size(),
                vars_vec, vars_values, set_only_non_debug_params)) {
            Nan::ThrowError("tesseract API Failed to initialize");
            return;
        }
        obj->Wrap(info.This());

        // Set properties on the returned object.
        info.This()->Set(Nan::New("lang").ToLocalChecked(), Nan::New<v8::String>(lang).ToLocalChecked());
        if (datapath) {
            info.This()->Set(Nan::New("datapath").ToLocalChecked(), Nan::New<v8::String>(datapath).ToLocalChecked());
        } else {
            info.This()->Set(Nan::New("datapath").ToLocalChecked(), Nan::Undefined());
        }

        info.GetReturnValue().Set(info.This());
    } else {
        // Create a new instance if the func wasn't called with new, heathen!
        const int argc = info.Length();
        std::vector<v8::Local<v8::Value>> argv;
        for (int i = 0; i< argc; i++) {
            argv.push_back(info[i]);
        }
        v8::Local<v8::Function> cons = Nan::New(NTApi::constructor());
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, &argv[0]).ToLocalChecked());
    }
}

NAN_METHOD(NTApi::ocr) {
    Callback *callback;
    v8::Local<v8::Object> options = Nan::New<v8::Object>();

    const int nargs = info.Length();
    if (nargs < 2 || nargs > 3) {
        Nan::ThrowTypeError("ocr takes two or three arguments. Signature: (pix, [options], callback)");
        return;
    }
    NTApi * obj = Nan::ObjectWrap::Unwrap<NTApi>(info.Holder());
    NTPix * image = Nan::ObjectWrap::Unwrap<NTPix>(info[0]->ToObject());
    if (info.Length() == 3) {
        Nan::MaybeLocal<v8::Object> MaybeOptions = Nan::To<v8::Object>(info[1]);
        if (!MaybeOptions.IsEmpty()) {
            options = MaybeOptions.ToLocalChecked();
        }
        callback = new Callback(info[2].As<Function>());
    } else {
        callback = new Callback(info[1].As<Function>());
    }
    AsyncQueueWorker(new OCRWorker(callback, image, obj->api, options));
    info.GetReturnValue().SetNull();
}

Nan::Persistent<v8::Function> & NTApi::constructor() {
    static Nan::Persistent<v8::Function> my_constructor;
    return my_constructor;
}
