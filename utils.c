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
