var addon = require('./addon');

function Git(){

};

Git.prototype.error = function(){
    return addon.Git.prototype.lastError();
};

Git.prototype.repo = function(name,desc){
    return addon.Git.prototype.createRepo(name,desc);
};

Git.prototype.delete = function(){
    addon.Git.prototype.deleteRepo();
};

Git.prototype.diff = function(){
    addon.Git.prototype.diff();
};

Git.prototype.add = function(path,buf){
    addon.Git.prototype.add(path,buf);
};

Git.prototype.commit = function(msg){
    addon.Git.prototype.commit('HEAD',msg);
};

Git.prototype.branch = function(name){
    addon.Git.prototype.createBranch(name);
};

Git.prototype.tag = function(name){
    addon.Git.prototype.createTag(name);
};

Git.prototype.blame = function(){
    addon.Git.prototype.blame();
};

module.exports = new Git();
