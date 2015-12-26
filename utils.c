#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "utils.h"


int find_free_meta_index(fs_t *fs)
{
	int i;
	for(i = 0; i<FILE_NUMBER; i++)
		if(!fs->meta[i].busy)
			return i;
	return ERR_NFM;
}

meta_t* get_meta_by_index(fs_t *fs, int index)
{
	return &fs->meta[index];
}

int find_free_block(fs_t *fs)
{
	int i;
	for(i = FIRST_BLOCK; i<BLOCK_NUMBER; i++)
	{
		if(fs->next[i]==BLOCK_FREE)
			return i;
	}
	return ERR_NFB;
}

void write_meta(fs_t *fs, meta_t *meta, int index)
{
	FILE *fsfile = fopen(FS_FILE, "r+");
	fseek(fsfile, index * sizeof(meta_t), SEEK_SET);
	fwrite(meta, 1, sizeof(meta_t), fsfile);
	fclose(fsfile);
}

void write_precedence_vector(fs_t *fs, meta_t *meta)
{
	FILE *fsfile = fopen(FS_FILE, "r+");
	int i = meta->start_block;
	fseek(fsfile, sizeof(meta_t)*FILE_NUMBER + i*sizeof(int), SEEK_SET);
	fwrite(&fs->next[i], sizeof(int), 1, fsfile);
	while (i!=-1)
	{
		i = fs->next[i];
		fseek(fsfile, sizeof(meta_t)*FILE_NUMBER + i*sizeof(int), SEEK_SET);
		fwrite(&fs->next[i], sizeof(int), 1, fsfile);
	}
	fclose(fsfile);
}

int get_data(fs_t *fs, meta_t *meta, char *buffer)
{
	FILE *fsfile = fopen(FS_FILE, "r");
	char *data = NULL;
	int current, gotBlocks, bytes;
	if(meta==NULL)
		return -1;
	data = (char*)malloc(sizeof(char)*meta->file_size);
	current = meta->start_block;
	while (current!=-1)
	{
		bytes = (meta->file_size-gotBlocks*BLOCK_SIZE)>BLOCK_SIZE?BLOCK_SIZE:(meta->file_size-gotBlocks*BLOCK_SIZE);
		fseek(fsfile, sizeof(meta_t)*FILE_NUMBER+BLOCK_NUMBER*sizeof(int)+current*BLOCK_SIZE,SEEK_SET);
		fread(data+gotBlocks*BLOCK_SIZE, 1, bytes, fsfile);
		gotBlocks++;
		current = fs->next[current];
	}
	fclose(fsfile);
	buffer = data;
	return meta->file_size;
}

int get_index(fs_t *fs, char *data, int size, char *filename)
{
	int i;
	for(i = 0; i<size/sizeof(int); i++)
		if(strcmp(fs->meta[((int*)data)[i]].name, filename) == 0)
			return ((int*)data)[i];
	return -1;
}

int get_meta(fs_t *fs, const char *path, meta_t **meta)
{
	meta_t *temp_meta = NULL;
	int size, index;
	char filename[FILENAME_LENGTH],*data =NULL, *start =NULL;
	char *ptr = (char*)malloc(strlen(path)*sizeof(char));
	strcpy(ptr,path);
	memset(filename, '\0', FILENAME_LENGTH);
	if(strcmp(path, "/") == 0)
	{
		*meta = get_meta_by_index(fs,0);
		return FIRST_BLOCK;
	}
	if(*ptr++=='/')
		temp_meta = get_meta_by_index(fs,0);
	else
		return -1;
	while ((ptr-path) != strlen(path))
	{
		if(temp_meta->file_size==0)
			return -1;
		size = get_data(fs,temp_meta, data);
		temp_meta = NULL;
		start = ptr;
		while ((*ptr++ != '/')&&((ptr-path)<strlen(path)));
		(ptr-path)<strlen(path)? strncpy(filename, start, ptr-start-1):strncpy(filename,start,ptr-start);
		index = get_index(fs,data, size, filename);
		if(index == -1)
			return -1;
		temp_meta = get_meta_by_index(fs,index);
		memset(filename, '\0', FILENAME_LENGTH);
		free(data);
	}

	*meta = temp_meta;
	return index;
}
