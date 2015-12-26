#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "fat_fs.h"
#include "utils.h"

int create_fs(fs_t *fs)
{
	int i;
	meta_t *metabuf;
	FILE *fsfile;
	fsfile = fopen(FS_FILE, "w+");
	printf("file opened\n");
	metabuf = (meta_t*) malloc(BUFFER_SIZE*sizeof(meta_t));
	memset(metabuf, '\0', BUFFER_SIZE*sizeof(meta_t));
	printf("memset\n");
	for(i = 0; i<FILE_NUMBER/BUFFER_SIZE; i++)
	{
		printf("%d\n", i);
		fwrite(metabuf, sizeof(meta_t),BUFFER_SIZE,fsfile); //check
	}
	printf("meta written\n");
	free(metabuf);

	int *vectorbuf;
	vectorbuf = (int*)malloc(BUFFER_SIZE*sizeof(int));
	memset(vectorbuf, 0 , BUFFER_SIZE*sizeof(int));
	for (i = 0; i < BLOCK_NUMBER/BUFFER_SIZE; i++) {
		fwrite(vectorbuf, sizeof(int), BUFFER_SIZE, fsfile);
	}
	printf("vector written\n");
	free(vectorbuf);

	char *databuf;
	databuf = (char*)malloc(BLOCK_SIZE*BUFFER_SIZE);
	memset(databuf, '\0', BLOCK_SIZE*BUFFER_SIZE);
	for (i = 0; i < BLOCK_NUMBER/BUFFER_SIZE; i++) {
		fwrite(databuf, BLOCK_SIZE, BUFFER_SIZE, fsfile);
	}
	printf("data written\n");
	free(databuf);
	fclose(fsfile);
	init_fs(fs);
	create_empty_file(fs,"/", 1);
	return 0;
}

int init_fs(fs_t *fs)
{
	int i;
	for(i=0;i<FILE_NUMBER; i++)
	{
		fs->meta[i].busy=0;
		fs->meta[i].is_folder=0;
		memset(fs->meta[i].name, '\0', FILENAME_LENGTH);
		fs->meta[i].start_block = -1;
		fs->meta[i].file_size = 0;
	}
	printf("fs initialized\n");
	return 0;
}

int create_empty_file(fs_t *fs, char *name, char is_folder)
{
	printf("in create_empty_file\n");
	int meta_index = find_free_meta_index(fs);
	printf("meta index found\n" );
	if(meta_index==ERR_NFM)
		return ERR_NFM;
	meta_t *meta = get_meta_by_index(fs, meta_index);
	printf("meta_t got\n" );
	//fill new meta
	meta->busy=1;
	meta->is_folder = is_folder;
	meta->file_size = 0;
	strcpy(meta->name, name);
	int start_block = find_free_block(fs);
	printf("free block found\n" );
	if(start_block==ERR_NFB)
	{
		printf("FREE BLOCK NOT FOUND\n" );
		return ERR_NFB;
	}
	meta->start_block = start_block;
	fs->next[start_block] = -1; //next block is eof
	write_meta(fs,meta, meta_index);
	printf("meta written\n");
	write_precedence_vector(fs,meta);
	printf("vector written\n");
	return meta_index;
}
