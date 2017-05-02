#include <v8.h>
#include <node.h>
#include <nan.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "api.h"
#include "pix.h"

using v8::Function;
using v8::FunctionTemplate;
using v8::Local;
using v8::Number;
using v8::Value;
using v8::Object;
using v8::String;
using v8::Array;
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
        OCRWorker(Callback *callback, NTPix * image, Local<Object> options) : AsyncWorker(callback), image(image), text(NULL) {
            // Defaults
            dataPath = "";
            lang = "eng";
            mode = tesseract::OEM_DEFAULT;
            vars_vec = NULL;
            vars_values = NULL;
            set_only_non_debug_params = true;
            psm = tesseract::PSM_SINGLE_BLOCK;
            rect.w = 0; rect.h = 0; rect.x = 0; rect.y = 0;

            // local keys
            Local<String> psm_key = Nan::New("psm").ToLocalChecked();
            Local<String> rect_key = Nan::New("rect").ToLocalChecked();
            Local<String> lang_key = Nan::New("lang").ToLocalChecked();
            Local<String> dataPath_key = Nan::New("dataPath").ToLocalChecked();
            
            // local option values
            Local<Value> local_psm = Get(options, psm_key).ToLocalChecked();
            Local<Value> local_rect = Get(options, rect_key).ToLocalChecked();
            Local<Value> local_dataPath = Get(options, dataPath_key).ToLocalChecked();
            Local<Value> local_lang = Get(options, lang_key).ToLocalChecked();

            // Parse Options
            if (!local_lang->IsUndefined()) {
                if (!local_lang->IsString() || !((lang = std::string(*Nan::Utf8String(local_lang))).length() == 3)) {
                    SetErrorMessage("OCR option lang must be a three character string");
                    return;
                }
            }

            if (!local_dataPath->IsUndefined()) {
                if (local_dataPath->IsString()) {
                    dataPath = *Nan::Utf8String(local_dataPath);
                } else {
                    SetErrorMessage("OCR option dataPath must be a string");
                    return;
                }
            }

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
                    Local<Array> rect_arr = local_rect.As<Array>();
                    Local<Value> local_rect_0 = Get(rect_arr, 0).ToLocalChecked();
                    Local<Value> local_rect_1 = Get(rect_arr, 1).ToLocalChecked();
                    Local<Value> local_rect_2 = Get(rect_arr, 2).ToLocalChecked();
                    Local<Value> local_rect_3 = Get(rect_arr, 3).ToLocalChecked();
                    bool all_ints = local_rect_0->IsNumber() && local_rect_1->IsNumber() && local_rect_2->IsNumber() && local_rect_3->IsNumber();
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
            if (ErrorMessage()) { return; }
            tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
            int r = api->Init(
                dataPath.size() > 0 ? dataPath.c_str() : NULL,
                lang.c_str(),
                mode,
                &configs[0], configs.size(),
                vars_vec, vars_values,
                set_only_non_debug_params
            );
            if (r) {
                SetErrorMessage("Tesseract API failed to initialize");
                return;
            }

            api->SetPageSegMode(psm);
            api->SetImage(image->image);
            if (rect.w > 0 && rect.h > 0) {
                api->SetRectangle(rect.x, rect.y, rect.w, rect.h);
            }
            text = api->GetUTF8Text();
        }

        void HandleOKCallback() {
          HandleScope scope;
          Local<Value> argv[] = { Null(), Nan::New<String>(text).ToLocalChecked() };
          callback->Call(2, argv);
        }

    private:
        NTPix * image;
        TessBaseAPI * api;
        std::string lang;
        std::string dataPath;
        char * text;
        Local<Object> options;
        tesseract::PageSegMode psm;
        RectBox rect;
        tesseract::OcrEngineMode mode;
        std::vector<char *> configs;
        GenericVector<STRING> *vars_vec;
        GenericVector<STRING> *vars_values;
        bool set_only_non_debug_params;
};

NAN_MODULE_INIT(NTApi::Init) {
    Nan::Set(target, Nan::New("ocr").ToLocalChecked(),
            Nan::GetFunction(Nan::New<FunctionTemplate>(NTApi::ocr)).ToLocalChecked());
}

NTApi::NTApi() {}
NTApi::~NTApi() {}

NAN_METHOD(NTApi::ocr) {
    Callback *callback;
    v8::Local<v8::Object> options = Nan::New<v8::Object>();

    const int nargs = info.Length();
    if (nargs < 2 || nargs > 3) {
        Nan::ThrowTypeError("ocr takes two or three arguments. Signature: (pix, [options], callback)");
        return;
    }

    if (info.Length() == 3) {
        Nan::MaybeLocal<Object> MaybeOptions = Nan::To<Object>(info[1]);
        if (!MaybeOptions.IsEmpty()) {
            options = MaybeOptions.ToLocalChecked();
        }
        callback = new Callback(info[2].As<Function>());
    } else {
        callback = new Callback(info[1].As<Function>());
    }
    
    if (!info[0]->IsObject()) {
        Nan::ThrowTypeError("the first argument to ocr must be a Pix object.");
        return;
    }

    NTPix * image = Nan::ObjectWrap::Unwrap<NTPix>(info[0]->ToObject());

    AsyncQueueWorker(new OCRWorker(callback, image, options));
    info.GetReturnValue().SetNull();
}
