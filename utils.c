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
	return &(fs->meta[index]);
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
	fwrite(meta, sizeof(meta_t),1 , fsfile);
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

int write_data(fs_t *fs, meta_t *meta, void *data, int size)
{
	int current_block=0, written_blocks=0, bytes_to_write=0, last_block =0;
	if(size == 0)
		return 0;
	FILE *fsfile = fopen(FS_FILE, "r+");
	current_block = meta->start_block;
	bytes_to_write = BLOCK_SIZE;
	while (bytes_to_write==BLOCK_SIZE)
	{
		bytes_to_write = (written_blocks*BLOCK_SIZE + BLOCK_SIZE) > size? (size-BLOCK_SIZE*written_blocks):BLOCK_SIZE;
		//printf("current=%d, bytes to write=%d, filesize=%d\n", current_block, bytes_to_write, meta->file_size);
		fseek(fsfile,sizeof(meta_t)*FILE_NUMBER+sizeof(int)*BLOCK_NUMBER + current_block*BLOCK_SIZE, SEEK_SET);
		//printf("after fseek\n");
		fwrite(data+written_blocks*BLOCK_SIZE, sizeof(char), bytes_to_write, fsfile);
		//printf("after fseek offset = %d\n", written_blocks*BLOCK_SIZE);
		last_block = current_block;
		fs->next[current_block] = find_free_block(fs);
		current_block = fs->next[current_block];
		//printf("new block = %d\n", current_block);
		written_blocks++;
	}
	fs->next[last_block] = -1;
	//printf("after while\n");
	if(bytes_to_write==8)
	{
		int f = *(((int*)data)+0*sizeof(int)), s = *(((int*)data)+1*sizeof(int));
		//printf("written to file first index %d\n", f);
		//printf("written to file second index %d\n", s);
	}
	//meta->file_size+=size;
	write_precedence_vector(fs,meta);
	//printf("after write vector\n");
	fclose(fsfile);
	return size;
}

char* get_filename(const char *path) {
	char *filename;
	char *slash = NULL;
	char *ptr = (char*)malloc((strlen(path)+1)*sizeof(char));
	strcpy(ptr,path);
	while (slash = *ptr == '/' ? ptr : slash, *ptr++ != '\0');
	filename = (char*)malloc(sizeof(char) * (ptr - slash));
	strncpy(filename, slash + 1, ptr - slash);
	return filename;
}

char* get_directory(const char *path) {
	char *directory;
	char *slash = NULL;
	char *ptr = (char*)malloc((strlen(path)+1)*sizeof(char));
	strcpy(ptr,path);
	while (slash = *ptr == '/' ? ptr : slash, *ptr++ != '\0');
	int pathlen = strlen(path)+1;
	int slashlen = strlen(slash)+1;
	if ((pathlen - slashlen) != 0) {
		directory = (char*)malloc(sizeof(char) * (pathlen - slashlen));
		strncpy(directory, path, pathlen - slashlen);
		directory[pathlen-slashlen] = '\0';
	}
	else {
		directory = (char*)malloc(sizeof(char) * 2);
		strcpy(directory, "/\0");
	}
	return directory;
}

int get_data(fs_t *fs, meta_t *meta, char **buffer)
{
	FILE *fsfile = fopen(FS_FILE, "r");
	char *data = NULL;
	int current, gotBlocks=0, bytes=0;
	if(meta==NULL)
		return -1;
	data = (char*)malloc(sizeof(char)*(meta->file_size));
	current = meta->start_block;
	while (current!=-1)
	{
		bytes = (meta->file_size-gotBlocks*BLOCK_SIZE)>BLOCK_SIZE?BLOCK_SIZE:(meta->file_size-gotBlocks*BLOCK_SIZE);
		//printf("before fseek current=%d, bytes to read=%d, filesize=%d\n", current, bytes, meta->file_size);
		fseek(fsfile, sizeof(meta_t)*FILE_NUMBER+BLOCK_NUMBER*sizeof(int)+current*BLOCK_SIZE,SEEK_SET);
		//printf("after fseek\n");
		fread(data+gotBlocks*BLOCK_SIZE, sizeof(char), bytes, fsfile);
		gotBlocks++;
		current = fs->next[current];
		//printf("next current=%d\n", current);
	}
	fclose(fsfile);
	if(bytes == 8)
	{
		int f = *((int*)data), s = *((int*)data + 1);
		//printf("read from file first index %d\n", f);
		//printf("read from file second index %d\n", s);
	}
	*buffer = data;
	return meta->file_size;
}

int get_index(fs_t *fs, char *data, int size, char *filename)
{
	int i=0;
	//printf("in get index meta[0].name=%s\n", fs->meta[0].name);
	//printf("in get index meta[1].name=%s\n", fs->meta[1].name);
	//printf("in get index meta[2].name=%s\n", fs->meta[2].name);
	//printf("index of new_folder via dir_data =%d\n", *((int *)data + sizeof(int)));
	//printf("in get index data=%d, size=%d, filename=%s\n", (int)data, size, filename);
	for(i = 0; i<size/sizeof(int); i++)
		if(strcmp(fs->meta[*((int *)data + i)].name, filename) == 0)
			return *((int *)data + i);
	return -1;
}

int get_meta(fs_t *fs, const char *path, meta_t **meta)
{

	meta_t *temp_meta = NULL;
	int size, index;
	char filename[FILENAME_LENGTH],*data =NULL, *start =NULL;
	//printf("before malloc\n");
	char *ptr = (char*)malloc((strlen(path)+1)*sizeof(char)); //strdup(path);
	strcpy(ptr,path);
	//printf("path=%s, ptr=%s\n", path, ptr);
	memset(filename, '\0', FILENAME_LENGTH);
	if(strcmp(path, "/") == 0)
	{
		*meta = get_meta_by_index(fs,0);
		return 0;
	}
	if(*ptr=='/')
	{
		//printf("in if\n");
		temp_meta = get_meta_by_index(fs,0);
	}
	else
		return -1;
//	while ((ptr-path) != strlen(path))
	//printf("before while\n");
	ptr++;
	while (*ptr!='\0')
	{
		if(temp_meta->file_size==0)
			return -1;
		size = get_data(fs,temp_meta, &data);
		//printf("after get_data size=%d, pointer=%d\n",size, (int)data);
		temp_meta = NULL;
		start = ptr;
		//while ((*ptr++ != '/')&&((ptr-path)<strlen(path)));
		while ((*ptr != '/')&&(*ptr != '\0')) ptr++;
		(*ptr != '\0')? strncpy(filename, start, ptr-start-1):strncpy(filename,start,ptr-start);
		//printf("before get index filename=%s\n", filename);
		index = get_index(fs,data, size, filename);
		//printf("after get index=%d\n",index);
		if(index == -1)
			return -1;
		temp_meta = get_meta_by_index(fs,index);
		//printf("temp_meta filename=%s\n",temp_meta->name);
		memset(filename, '\0', FILENAME_LENGTH);
		free(data);
	}
	//printf("after while \n");
	*meta = temp_meta;
	return index;
}
