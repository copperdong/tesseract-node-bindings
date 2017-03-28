# Bindings from Node to Tesseract

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
const api = new tesseract.BaseAPI({ lang: 'eng' });

console.log(`Tesseract Version: ${api.version}`);

tesseract.readImage('./helloWorld.tiff', (err, img) => {
  api.ocr(img, (err, text) => {
    console.log(text);
  });
});
```
