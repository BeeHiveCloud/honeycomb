var Promise = require('bluebird');

try {
  rawApi = require('../build/Release/addon');
}
catch (e) {
  rawApi = require('../build/Debug/addon');
}

function Honeycomb() {

};

module.exports = Honeybee;
