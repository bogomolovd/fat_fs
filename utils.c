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
	init(fs);
	//add_file("/", buf, 0, 1);
	return 0;
}
int init(fs_t *fs)
{
	int i;
	for(i=0;i<FILE_NUMBER; i++)
	{
		fs->meta[i].busy=0;
		fs->meta[i].is_folder=0;
		memset(fs->meta[i].name, '\0', FILENAME_LENGTH);
		fs->meta[i].start_block = -1;
		fs->meta[i].block_size = 0;
	}
	printf("fs initialized\n");
	return 0;
}
