/**
 * Export honeybee
 */

 try {
   rawApi = require('../build/Release/addon');
 }
 catch (e) {
   rawApi = require('../build/Debug/addon');
 }


var honeybee = require('./honeybee');

module.exports = new honeybee();
