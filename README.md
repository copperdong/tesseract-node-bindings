# Bindings from Node to Tesseract

[![Build Status](https://api.travis-ci.org/schmidmt/tesseract-node-bindings.svg?branch=master)](https://travis-ci.org/schmidmt/tesseract-node-bindings)
[[![Coverage](http://codecov.io/github/schmidmt/tesseract-node-bindings/coverage.svg?branch=master)](https://codecov.io/gh/schmidmt/tesseract-node-bindings)

## Leptonica Pix Interface
Loading in an image is possible via the `readImage` function:
```javascript
conyyst tesseract = require('tesseract-node-bindings');

tesseract.readImage('./helloWorld.tiff', (err, img) => {
  console.log(err, img);
});
```

## Base API

To run tesseract, you'll need to create an instance of the baseApi,

```javascript
const tesseract = require('tesseract-node-bindings');

console.log(`Tesseract Version: ${api.version}`);

tesseract.readImage('./helloWorld.tiff', (err, img) => {
  tesseract.ocr(img, (err, text) => {
    console.log(text);
  });
});
```
