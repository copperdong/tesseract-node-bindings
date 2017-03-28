#include <nan.h>
#include <node.h>
#include <tesseract/baseapi.h>

#include "pix.h"
#include "api.h"

using namespace v8;
using namespace node;

using tesseract::PageSegMode;


NAN_MODULE_INIT(InitAll) {
    NTApi::Init(target);
    NTPix::Init(target);

    target->Set(Nan::New("PSM_OSD_ONLY").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_OSD_ONLY));
    target->Set(Nan::New("PSM_AUTO_OSD").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_AUTO_OSD));
    target->Set(Nan::New("PSM_AUTO_ONLY").ToLocalChecked(),              Nan::New<v8::Number>(PageSegMode::PSM_AUTO_ONLY));
    target->Set(Nan::New("PSM_AUTO").ToLocalChecked(),                   Nan::New<v8::Number>(PageSegMode::PSM_AUTO));
    target->Set(Nan::New("PSM_SINGLE_COLUMN").ToLocalChecked(),          Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_COLUMN));
    target->Set(Nan::New("PSM_SINGLE_BLOCK_VERT_TEXT").ToLocalChecked(), Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_BLOCK_VERT_TEXT));
    target->Set(Nan::New("PSM_SINGLE_BLOCK").ToLocalChecked(),           Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_BLOCK));
    target->Set(Nan::New("PSM_SINGLE_LINE").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_LINE));
    target->Set(Nan::New("PSM_SINGLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_WORD));
    target->Set(Nan::New("PSM_CIRCLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_CIRCLE_WORD));
    target->Set(Nan::New("PSM_SINGLE_WORD").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_WORD));
    target->Set(Nan::New("PSM_SINGLE_CHAR").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SINGLE_CHAR));
    target->Set(Nan::New("PSM_SPARSE_TEXT").ToLocalChecked(),            Nan::New<v8::Number>(PageSegMode::PSM_SPARSE_TEXT));
    target->Set(Nan::New("PSM_SPARSE_TEXT_OSD").ToLocalChecked(),        Nan::New<v8::Number>(PageSegMode::PSM_SPARSE_TEXT_OSD));
    target->Set(Nan::New("PSM_RAW_LINE").ToLocalChecked(),               Nan::New<v8::Number>(PageSegMode::PSM_RAW_LINE));


}

NODE_MODULE(tesseract, InitAll)
