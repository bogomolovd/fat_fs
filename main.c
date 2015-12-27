#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "fat_fs.h"
#include "utils.h"
#include "fat_fs.h"

fs_t fs;

static int fat_getattr(const char *path, struct stat *stbuf)
{
        printf("getattr path = %s\n", path);
        int res = 0;
        //char *data;
        meta_t *meta=NULL;
        int index = get_meta(&fs,path,&meta);
        if(index==-1)
          return -ENOENT;
        memset(stbuf, 0, sizeof(struct stat));
      	if (meta->is_folder == 1) {
      		stbuf->st_mode = S_IFDIR | 0755;
      		stbuf->st_nlink = 2;
      	}
      	else {
      		stbuf->st_mode = S_IFREG | 0444;
      		stbuf->st_nlink = 1;
      		stbuf->st_size = meta->file_size;
      	}
      	//stbuf->st_mode = stbuf->st_mode | 0777;
      	return res;
}

static int fat_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
      int i = 0;
    	int index = 0;
    	(void) offset;
    	(void) fi;
    	char *data;
    	int size;

    	filler(buf, ".", NULL, 0);
    	filler(buf, "..", NULL, 0);

    	meta_t *meta;
    	index = get_meta(&fs,path, &meta);
    	if (index == -1)
    		return -ENOENT;
    	size = get_data(&fs,meta, &data);
    	if (meta->file_size == 0)
    		return 0;
    	for (i = 0; i < size / sizeof(int); i++)
    		filler(buf, fs.meta[((int*)data)[i]].name, NULL, 0);
      return 0;
}

static int fat_open(const char *path, struct fuse_file_info *fi)
{
      meta_t *meta;
    	int index = 0;
    	index = get_meta(&fs,path, &meta);
    	if (index == -1)
  		  return -ENOENT;
      return 0;
}

static int fat_opendir(const char *path, struct fuse_file_info *fi)
{
    	meta_t *meta;
    	int index = 0;
    	index = get_meta(&fs,path, &meta);
    	if (index == -1)
    		return -ENOENT;
    	return 0;
}

static int fat_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
      size_t len;
      char *data;
    	meta_t *meta;
    	get_meta(&fs,path, &meta);
    	if (meta == NULL)
    		return -ENOENT;

    	len = get_data(&fs,meta, &data);

    	if (len < 0)
    		return -ENOENT;
    	if (offset < len) {
    		if (offset + size > len)
    			size = len - offset;
    		memcpy(buf, data + offset, size);
    	}
    	else
    		size = 0;
      return size;
}

static int fat_mkdir(const char *path, mode_t mode)
{
	int res;
	res = create_file(&fs,path, 1);
	if (res != 0)
		return -1;
	return 0;
}

static int fat_createfile (const char *path, mode_t mode, struct fuse_file_info *fi) {
	/*int res;
	res = mkfile(path, 0);
	if (res != 0)
		return -1;*/
	return 0;
}

static struct fuse_operations fat_oper = {
        .getattr        = fat_getattr,
        .readdir        = fat_readdir,
        .open           = fat_open,
        .read           = fat_read,
        .opendir        = fat_opendir,
        .mkdir          = fat_mkdir,
        .create         = fat_createfile,
};
int main(int argc, char *argv[])
{
        create_fs(&fs);
        return fuse_main(argc, argv, &fat_oper, NULL);
}
