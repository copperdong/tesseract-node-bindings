const bluebird = require('bluebird');
const tesseract = require('bindings')('tesseract.node');
const version = require('./package.json').version;

tesseract.version = version;

bluebird.promisifyAll(tesseract, {
  filter: (name, func, target, passesDefaultFilter) => {
    const dontPromisify = [
      'BaseAPI',
    ];
    return passesDefaultFilter && dontPromisify.indexOf(name) === -1;
  }
});

module.exports = tesseract;
