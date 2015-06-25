var addon;

try {
  addon = require('../build/Debug/addon');
}
catch (e) {
  console.log(e);
}

module.exports = addon;
