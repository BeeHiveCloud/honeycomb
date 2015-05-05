var _ = require('lodash');

try {
  rawApi = require('../build/Release/addon');
}
catch (e) {
  rawApi = require('../build/Debug/addon');
}

function Honeycomb(){
  this.open = _.bind(this.open, this);
  this.close = _.bind(this.close, this);
  this.newRepo = _.bind(this.newRepo, this);
  this.delRepo = _.bind(this.delRepo, this);
  this.setRepo = _.bind(this.setRepo, this);
};

Honeycomb.prototype.open = function(host,db,user,password,port){
  return new Promise(function(resolve,reject){
    if(rawApi.MySQL.Open(host,db,user,password,port))
      resolve();
    else
      reject(Error(rawApi.MySQL.LastError()));
  });
};

Honeycomb.prototype.close = function(){
    rawApi.MySQL.Close();
};

Honeycomb.prototype.newRepo = function(name,desc){
    rawApi.MySQL.CreateRepo(1,name,desc);
};

Honeycomb.prototype.delRepo = function(){
    rawApi.MySQL.DeleteRepo();
};

Honeycomb.prototype.setRepo = function(rid){
    rawApi.MySQL.SetRepo(rid);
};

module.exports = Honeycomb;
