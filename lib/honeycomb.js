var _ = require('lodash');

function Honeycomb(){
  this.Git = require('./git');
  this.MySQL = require('./mysql');
};

module.exports = Honeycomb;
