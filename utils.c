#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "utils.h"

int create_fs(fs_t *fs)
{
  int i;
  char *buffer;
  FILE *fsfile;
  fsfile = fopen(FS_FILE, "w+");
  printf("file opened\n");
  buffer = (char*) malloc(sizeof(meta_t));
  memset(buffer, '\0', sizeof(meta_t));
  printf("memset\n");
  for(i = 0; i<FILE_NUMBER; i++)
  {
    printf("%d\n", i);
    fwrite(buffer, sizeof(meta_t),1,fsfile); //check
  }
  printf("meta written\n");
  free(buffer);

  buffer = (char*)malloc(sizeof(int));
	memset(buffer, '\0', sizeof(int));
	for (i = 0; i < BLOCK_NUMBER; i++) {
		fwrite(buffer, sizeof(int), 1, fsfile);
	}
  printf("vector written\n");
	free(buffer);
	buffer = (char*)malloc(BLOCK_SIZE);
	memset(buffer, '\0', BLOCK_SIZE);
	for (i = 0; i < BLOCK_NUMBER; i++) {
		fwrite(buffer, BLOCK_SIZE, 1, fsfile);
	}
  printf("data written\n");
	free(buffer);
	fclose(fsfile);
	//init_fs();
	//add_file("/", buf, 0, 1);
	return 0;
}
