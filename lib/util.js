var fs = require("fs");

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
