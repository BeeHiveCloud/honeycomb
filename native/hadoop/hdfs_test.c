#include "hdfs_test.h"

#include <stdio.h>
#include <stdlib.h>

int hdfs_test(){
  //putenv("PATH=%CLASSPATH%;%PATH%");
  //printf("PATH=%s \n", getenv("PATH"));

  hdfsFS fs = hdfsConnect("default", 0);
  if (!fs) {
	  printf("Oops! Failed to connect to hdfs!\n");
	  return -1;
  }else
	  printf("connect to hdfs!\n");

  const char* writePath = "/testfile.txt";
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

int main(int argc, char **argv) {
  return hdfs_test();
}
