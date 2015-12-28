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
	metabuf = (meta_t*) malloc(BUFFER_SIZE*sizeof(meta_t));
	memset(metabuf, '\0', BUFFER_SIZE*sizeof(meta_t));
	for(i = 0; i<FILE_NUMBER/BUFFER_SIZE; i++)
	{
		fwrite(metabuf, sizeof(meta_t),BUFFER_SIZE,fsfile); //check
	}
	free(metabuf);

	int *vectorbuf;
	vectorbuf = (int*)malloc(BUFFER_SIZE*sizeof(int));
	memset(vectorbuf, 0 , BUFFER_SIZE*sizeof(int));
	for (i = 0; i < BLOCK_NUMBER/BUFFER_SIZE; i++) {
		fwrite(vectorbuf, sizeof(int), BUFFER_SIZE, fsfile);
	}
	free(vectorbuf);

	char *databuf;
	databuf = (char*)malloc(BLOCK_SIZE*BUFFER_SIZE);
	memset(databuf, '\0', BLOCK_SIZE*BUFFER_SIZE);
	for (i = 0; i < BLOCK_NUMBER/BUFFER_SIZE; i++) {
		fwrite(databuf, BLOCK_SIZE, BUFFER_SIZE, fsfile);
	}
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
	return 0;
}

int create_empty_file(fs_t *fs, char *name, char is_folder)
{
	int meta_index = find_free_meta_index(fs);
	//printf("empty: found free meta index=%d, name=%s, is_folder=%d\n",meta_index, name, (int)is_folder);
	if(meta_index==ERR_NFM)
		return ERR_NFM;
	meta_t *meta = get_meta_by_index(fs, meta_index);
	//fill new meta
	meta->busy=1;
	meta->is_folder = is_folder;
	meta->file_size = 0;
	strcpy(meta->name, name);
	//printf("empty: meta name=%s\n",meta->name);
	int start_block = find_free_block(fs);
	if(start_block==ERR_NFB)
	{
		//printf("FREE BLOCK NOT FOUND\n" );
		return ERR_NFB;
	}
	meta->start_block = start_block;
	fs->next[start_block] = -1; //next block is eof
	write_meta(fs,meta, meta_index);
	//printf("empty: meta name=%s",meta->name);
	write_precedence_vector(fs,meta);
	return meta_index;
}

int create_file(fs_t *fs, const char *path, int type)
{
	//printf("in create_file, path=%s\n", path );
	meta_t *dir_meta;
	char *dir_data, *new_dir_data;
	char *path_to_dir = get_directory(path);
	//printf("directory=%s\n", path_to_dir);
	char *filename = get_filename(path);
	//printf("filename=%s\n", filename);
	int dir_index = get_meta(fs,path_to_dir,&dir_meta);
	//printf("directory index=%d\n", dir_index);
	int data_size = get_data(fs, dir_meta, &dir_data);
	//printf("data size=%d\n", data_size);
	new_dir_data = (char*)malloc(data_size+sizeof(int));
	//printf("new dir data ptr=%p\n", new_dir_data);
	memcpy(new_dir_data, dir_data, data_size);
	//printf("after memcpy\n");
	int created_file_index = create_empty_file(fs, filename, type);
	//printf("created file index=%d\n", created_file_index);
	*((int *)(new_dir_data + data_size)) = created_file_index;
	write_data(fs,dir_meta, new_dir_data, data_size + sizeof(int));
	//printf("after write data\n");
	dir_meta->file_size = data_size + sizeof(int);
	write_meta(fs,dir_meta, dir_index);
	//printf("after write meta dir size=%d\n", dir_meta->file_size);
	free(dir_data);
	free(filename);
	free(path_to_dir);
	return 0;
}
