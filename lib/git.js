var _ = require('lodash');
var addon = require('./addon');

function Git(){
  this.newRepo = _.bind(this.newRepo, this);
  this.delRepo = _.bind(this.delRepo, this);
  this.setRepo = _.bind(this.setRepo, this);
  this.diff = _.bind(this.diff, this);
  this.add = _.bind(this.add, this);
  this.commit = _.bind(this.commit, this);
};

Git.prototype.newRepo = function(name,desc){
    return addon.GIT.CreateRepo(1,name,desc);
};

Git.prototype.delRepo = function(){
    addon.GIT.DeleteRepo();
};

Git.prototype.setRepo = function(rid){
    addon.GIT.SetRepo(rid);
};

Git.prototype.diff = function(){
    addon.GIT.Diff();
};

Git.prototype.add = function(path,buf){
    addon.GIT.Add(path,buf);
};

Git.prototype.commit = function(msg){
    addon.GIT.Commit('HEAD',msg);
};

module.exports = new Git();
