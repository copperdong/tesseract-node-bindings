const test = require('tape');
const fs = require('fs');

test('module', (t) => {
    const tesseract = require('..');
    t.equal(typeof tesseract.readImage, 'function', 'readImage should be a function');
    t.equal(typeof tesseract.BaseAPI, 'function', 'BaseAPI should be a function');
    t.end();
});

test('BaseAPI Version', (t) => {
    const tesseract = require('..');
    const api = new tesseract.BaseAPI();
    t.ok(api.version, 'Version must be assigned');
    t.end();
});

test('BaseAPI Properties', (t) => {
    const tesseract = require('..');
    const api = new tesseract.BaseAPI({ lang: 'eng' });
    t.equal(api.lang, 'eng', 'Language must be set');
    t.end();
});

test('Pix', (t) => {
    const tesseract = require('..');
    const imgBuffer = fs.readFileSync('./helloWorld.png');

    t.plan(4);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        t.notOk(err, 'No Error should have occured');
        t.ok(img, 'img should be a valid object.');
    });

    tesseract.readImage(imgBuffer, (err, img) => {
        t.notOk(err, 'Buffer: No Error should have occured');
        t.ok(img, 'Buffer: img should be a valid object.');
    });
});

test('BaseAPI ocr', (t) => {
    const tesseract = require('..');
    const api = new tesseract.BaseAPI({ lang: 'eng' });
    t.plan(4);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        api.ocr(img, (ocrErr, text) => {
            t.equal(text.trim(), 'Hello, World.');
            t.error(ocrErr, 'No error should be set');
        });
        t.throws(() => api.ocr(img), TypeError, 'Incorrect call throws TypeError');
    });

    const api_no_new = tesseract.BaseAPI({ lang: 'eng' });
    t.ok(api_no_new, 'Can instanciate without new');
});
 
test('BaseAPI ocr PSM', (t) => {
    const tesseract = require('..');
    const api = new tesseract.BaseAPI({ lang: 'eng' });
    t.plan(4);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        api.ocr(img, { psm: tesseract.PSM_SINGLE_LINE }, (ocrErr, text) => {
            t.error(ocrErr, 'callback error should not be defined');
            t.equal(text.trim(), 'Hello, World.');
        });

        api.ocr(img, { psm: 'NOT VALID' }, (ocrErr, text) => {
            t.ok(ocrErr, 'Error is set in callback');
            t.notOk(text, 'Text should be empty');
        });
    });
});

test('BaseAPI ocr rect', (t) => {
    const tesseract = require('..');
    const api = new tesseract.BaseAPI({ lang: 'eng' });
    t.plan(6);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        api.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: [80, 60, 111, 106] }, (ocrErr, text) => {
            t.equal(text.trim(), 'Hello');
            t.error(ocrErr, 'No error should have been set');
            global.gc();
        });
        api.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: [80, 60, 'Bananas', 106] }, (ocrErr, text) => {
            t.ok(ocrErr, 'An error should be set on callback');
            t.error(text, 'No text should have been set');
        });
        api.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: 'Bananas' }, (ocrErr, text) => {
            t.ok(ocrErr, 'An error should be set on callback');
            t.error(text, 'No text should have been set');
        });
    });
});
