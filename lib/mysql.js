var Promise = require('bluebird');
var addon = require('./addon');

function MySQL(){

};

MySQL.prototype.open = function(host,db,user,password,port){
  return new Promise(function(resolve,reject){
    if(addon.MySQL.prototype.open(host,db,user,password,port))
      resolve();
    else
      reject(Error(addon.MySQL.prototype.lastError()));
  });
};

MySQL.prototype.close = function(){
    addon.MySQL.prototype.close();
};

MySQL.prototype.set = function(name,value){
    return addon.MySQL.prototype.set(name,value);
};

MySQL.prototype.get = function(name){
    return addon.MySQL.prototype.get(name);
};

module.exports = new MySQL();
