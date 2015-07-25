var _ = require('lodash');
var addon = require('./addon');

function Git(){
  this.init = _.bind(this.init, this);
  this.close = _.bind(this.close, this);
  this.error = _.bind(this.error, this);
  this.repo = _.bind(this.repo, this);
  this.delete = _.bind(this.delete, this);
  this.diff = _.bind(this.diff, this);
  this.add = _.bind(this.add, this);
  this.commit = _.bind(this.commit, this);
  this.branch = _.bind(this.branch, this);
  this.tag = _.bind(this.tag, this);
  this.status = _.bind(this.status, this);
};

Git.prototype.init = function(){
    return addon.GIT.Init();
};

Git.prototype.close = function(){
    return addon.GIT.LastError();
};

Git.prototype.error = function(){
    return addon.GIT.Close();
};

Git.prototype.repo = function(name,desc){
    return addon.GIT.CreateRepo(name,desc);
};

Git.prototype.delete = function(){
    addon.GIT.DeleteRepo();
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

Git.prototype.branch = function(msg){
    addon.GIT.Commit('HEAD',msg);
};

Git.prototype.tag = function(msg){
    addon.GIT.Commit('HEAD',msg);
};

Git.prototype.status = function(msg){
    addon.GIT.Commit('HEAD',msg);
};

module.exports = new Git();
