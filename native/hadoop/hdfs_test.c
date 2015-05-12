#include "hdfs_test.h"

#include <stdio.h>
#include <stdlib.h>

int hdfs_test(){

  putenv("CLASSPATH=");

  hdfsFS fs = hdfsConnect("localhost", 50070);
  const char* writePath = "/tmp/testfile.txt";
  hdfsFile writeFile = hdfsOpenFile(fs, writePath, O_WRONLY |O_CREAT, 0, 0, 0);
  if(!writeFile) {
        printf("Failed to open %s for writing!\n", writePath);
        return -1;
  }
  char* buffer = "Hello, World!";
  tSize num_written_bytes = hdfsWrite(fs, writeFile, (void*)buffer, strlen(buffer)+1);
  if (hdfsFlush(fs, writeFile)) {
         printf("Failed to 'flush' %s\n", writePath);
         return -1;
  }
  hdfsCloseFile(fs, writeFile);
  return 0;
}
