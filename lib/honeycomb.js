var addon;

try {
  addon = require('../build/addon');
}
catch (e) {
  console.log(e);
}

function Honeycomb(){
  this.Git = addon.Git;
  this.MySQL = addon.MySQL;
};

module.exports = Honeycomb;
