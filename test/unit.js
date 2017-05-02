const test = require('tape');
const fs = require('fs');

test('module', (t) => {
    const tesseract = require('..');
    t.equal(typeof tesseract.readImage, 'function', 'readImage should be a function');
    t.equal(typeof tesseract.ocr, 'function', 'ocr should be a function');
    t.end();
});

test('Version', (t) => {
    const tesseract = require('..');
    t.ok(tesseract.version, 'version must be assigned');
    t.ok(tesseract.tesseractVersion, 'tesseractVersion must be assigned');
    t.end();
});

test('Pix', (t) => {
    const tesseract = require('..');
    const imgBuffer = fs.readFileSync('./helloWorld.png');

    t.plan(7);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        t.notOk(err, 'No Error should have occured');
        t.ok(img, 'img should be a valid object.');
        t.equal(img.width, 474);
        t.equal(img.height, 199);
        t.equal(img.depth, 32);
    });

    tesseract.readImage(imgBuffer, (err, img) => {
        t.notOk(err, 'Buffer: No Error should have occured');
        t.ok(img, 'Buffer: img should be a valid object.');
    });
});

test('Pix Failure', (t) => {
  const tesseract = require('..');
  t.plan(2);
  tesseract.readImage(Buffer.from('0123456789', 'hex'), (err, img) => {
    t.ok(err, 'expected an error');
    t.notOk(img, 'There should be no image');
  });
});

test('ocr', (t) => {
    const tesseract = require('..');
    tesseract.readImage('../helloWorld.png', (err, img) => {
        tesseract.ocr(img, (ocrErr, text) => {
            t.equal(text.trim(), 'Hello, World.');
            t.error(ocrErr, 'No error should be set');
            t.end();
        });
    });
});
 
test('ocr psm', (t) => {
    const tesseract = require('..');
    t.plan(4);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        tesseract.ocr(img, { lang: 'eng',  psm: tesseract.PSM_SINGLE_LINE }, (ocrErr, text) => {
            t.error(ocrErr, 'callback error should not be defined');
            t.equal(text.trim(), 'Hello, World.');
        });

        tesseract.ocr(img, { psm: 'NOT VALID' }, (ocrErr, text) => {
            t.ok(ocrErr, 'Error is set in callback');
            t.notOk(text, 'Text should be empty');
        });
    });
});

test('ocr rect', (t) => {
    const tesseract = require('..');
    t.plan(6);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        tesseract.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: [80, 60, 111, 106] }, (ocrErr, text) => {
            t.equal(text.trim(), 'Hello');
            t.error(ocrErr, 'No error should have been set');
            global.gc();
        });
        tesseract.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: [80, 60, 'Bananas', 106] }, (ocrErr, text) => {
            t.ok(ocrErr, 'An error should be set on callback');
            t.error(text, 'No text should have been set');
        });
        tesseract.ocr(img, { psm: tesseract.PSM_SINGLE_LINE, rect: 'Bananas' }, (ocrErr, text) => {
            t.ok(ocrErr, 'An error should be set on callback');
            t.error(text, 'No text should have been set');
        });
    });
});

test('Empty', (t) => {
    const tesseract = require('..');
    tesseract.readImage('../helloWorld.png', (err, img) => {
        tesseract.ocr(img, { rect: [0, 0, 1, 1], psm: tesseract.PSM_SINGLE_LINE }, (ocrErr, text) => {
            t.equal(text.trim(), '', 'Empty text should be returned');
            t.error(ocrErr, 'No error should have been set');
            t.end();
        });
    });
});

test('Multiple Concurrent OCRs', (t) => {
    const tesseract = require('..');
    t.plan(4);
    tesseract.readImage('../helloWorld.png', (err, img) => {
        tesseract.ocr(img, { rect: [80, 60, 111, 106], psm: tesseract.PSM_SINGLE_LINE }, (ocrErr, text) => {
            t.equal(text.trim(), 'Hello', 'First rect should return Hello');
            t.error(ocrErr, 'No error should have been set');
        });
        tesseract.ocr(img, { rect: [200, 60, 230, 106], psm: tesseract.PSM_SINGLE_LINE }, (ocrErr, text) => {
            t.equal(text.trim(), 'World.', 'Second rect should return World.');
            t.error(ocrErr, 'No error should have been set');
        });
    });
});
