#include <nan.h>
#include <node.h>
#include <tesseract/baseapi.h>

#include "pix.h"
#include "api.h"

using namespace v8;
using namespace node;

using tesseract::PageSegMode;


NAN_MODULE_INIT(InitAll) {
    std::string version = tesseract::TessBaseAPI::Version();
    Nan::Set(target, Nan::New<v8::String>("tesseractVersion").ToLocalChecked(), Nan::New<v8::String>(version).ToLocalChecked());

    Nan::Set(target, Nan::New("PSM_OSD_ONLY").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_OSD_ONLY));
    Nan::Set(target, Nan::New("PSM_AUTO_OSD").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_AUTO_OSD));
    Nan::Set(target, Nan::New("PSM_AUTO_ONLY").ToLocalChecked(),              Nan::New<v8::Number>(PageSegMode::PSM_AUTO_ONLY));
    Nan::Set(target, Nan::New("PSM_AUTO").ToLocalChecked(),                   Nan::New<v8::Number>(PageSegMode::PSM_AUTO));
    Nan::Set(target, Nan::New("PSM_SINGLE_COLUMN").ToLocalChecked(),          Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_COLUMN));
    Nan::Set(target, Nan::New("PSM_SINGLE_BLOCK_VERT_TEXT").ToLocalChecked(), Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_BLOCK_VERT_TEXT));
    Nan::Set(target, Nan::New("PSM_SINGLE_BLOCK").ToLocalChecked(),           Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_BLOCK));
    Nan::Set(target, Nan::New("PSM_SINGLE_LINE").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_LINE));
    Nan::Set(target, Nan::New("PSM_SINGLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_WORD));
    Nan::Set(target, Nan::New("PSM_CIRCLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_CIRCLE_WORD));
    Nan::Set(target, Nan::New("PSM_SINGLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_WORD));
    Nan::Set(target, Nan::New("PSM_SINGLE_CHAR").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_CHAR));
    Nan::Set(target, Nan::New("PSM_SPARSE_TEXT").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SPARSE_TEXT));
    Nan::Set(target, Nan::New("PSM_SPARSE_TEXT_OSD").ToLocalChecked(),        Nan::New<v8::Number>(PageSegMode::PSM_SPARSE_TEXT_OSD));
#if (TESSERACT_VERSION & 0xFF0000 >= 3 && TESSERACT_VERSION & 0x00FF00 > 3)
    Nan::Set(target, Nan::New("PSM_RAW_LINE").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_RAW_LINE));
#endif

    NTApi::Init(target);
    NTPix::Init(target);
}

NODE_MODULE(tesseract, InitAll)
