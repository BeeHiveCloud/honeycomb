var addon = require('./addon');

function Honeycomb(){
  this.Git = addon.Git;
  this.MySQL = addon.MySQL;
};

module.exports = Honeycomb;
