var addon;

try {
  addon = require('../build/addon');
}
catch (e) {
  console.log(e);
}

module.exports = addon;
