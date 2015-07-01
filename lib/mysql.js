var Promise = require('bluebird');
var _ = require('lodash');
var addon = require('./addon');

function MySQL(){
  this.open = _.bind(this.open, this);
  this.close = _.bind(this.close, this);
  this.set = _.bind(this.set, this);
  this.get = _.bind(this.get, this);
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

MySQL.prototype.set = function(name,value){
    return addon.MySQL.Set(name,value);
};

MySQL.prototype.get = function(name){
    return addon.MySQL.Get(name);
};

module.exports = new MySQL();
