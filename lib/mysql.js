var Promise = require('bluebird');
var _ = require('lodash');
var addon = require('./addon');

function MySQL(){
  this.open = _.bind(this.open, this);
  this.close = _.bind(this.close, this);
};

MySQL.prototype.open = function(host,db,user,password,port){
  return new Promise(function(resolve,reject){
    if(addon.MySQL.Open(host,db,user,password,port))
      resolve();
    else
      reject(Error(addon.MySQL.LastError()));
  });
};

MySQL.prototype.close = function(){
    addon.MySQL.Close();
};

module.exports = new MySQL();
