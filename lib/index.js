/**
 * Export honeybee
 */

 try {
   rawApi = require("../build/Release/rawapi");
 }
 catch (e) {
   rawApi = require("../build/Debug/rawapi");
 }

var honeybee = require('./honeybee');

module.exports = new honeybee();
