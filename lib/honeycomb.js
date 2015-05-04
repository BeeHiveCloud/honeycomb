var fs = require("fs");
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

function walkSync(dir, filelist) {
  if( dir[dir.length-1] != '/') dir=dir.concat('/')

  var fs = fs || require('fs'),
      files = fs.readdirSync(dir);
  filelist = filelist || [];
  files.forEach(function(file) {
    if (fs.statSync(dir + file).isDirectory()) {
      filelist = walkSync(dir + file + '/', filelist);
    }
    else {
      filelist.push(dir+file);
    }
  });
  return filelist;
};

function loadDir(dir){
  var files = walkSync(dir);
  for( i=0; i < files.length; i++){
    var blob = fs.readFileSync(files[i],'utf8');
    files[i] = files[i].substring(1);
    var oid = rawApi.MySQL.Add('/'+files[i], blob );
  }
};


module.exports = Honeycomb;
