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
		//printf("getattr before get_meta\n");
        int index = get_meta(&fs,path,&meta);
		//printf("getattr after get_meta\n");
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
      printf("readdir path = %s, offset=%d\n", path, offset);
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
		//printf("meta ptr=%d", (int)meta);
    	if (meta->file_size == 0)
    		return 0;
    	for (i = 0; i < size / sizeof(int); i++)
    		filler(buf, fs.meta[((int*)data)[i]].name, NULL, 0);
      return 0;
}

static int fat_open(const char *path, struct fuse_file_info *fi)
{
      printf("open path = %s\n", path);
      meta_t *meta;
    	int index = 0;
    	index = get_meta(&fs,path, &meta);
    	if (index == -1)
  		  return -ENOENT;
      return 0;
}

static int fat_opendir(const char *path, struct fuse_file_info *fi)
{
      printf("opendir path = %s\n", path);
    	meta_t *meta;
    	int index = 0;
    	index = get_meta(&fs,path, &meta);
    	if (index == -1)
    		return -ENOENT;
    	return 0;
}

static int fat_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
      printf("read path = %s, size=%d, offset=%d\n", path, size, offset);
      char *data;
    	meta_t *meta;
    	get_meta(&fs,path, &meta);
    	if (meta == NULL)
    		return -ENOENT;

    	int len = get_data_with_offset(&fs,meta, &data,size,offset);
		printf("len=%d\n",len);

    	if (len < 0)
    		return -ENOENT;
		memcpy(buf, data, len);
		printf("%s", buf);
    	/*if (offset < len) {
    		if (offset + size > len)
    			size = len - offset;
    		
    	}
    	else
    		size = 0;*/
      return len;
}

static int fat_mkdir(const char *path, mode_t mode)
{
  printf("mkdir path = %s\n", path);
	int res;
	res = create_file(&fs,path, 1);
	if (res != 0)
		return -1;
	return 0;
}

static int fat_create (const char *path, mode_t mode, struct fuse_file_info *fi) {
  printf("create path = %s\n", path);
	int res;
	res = create_file(&fs,path, 0);
	if (res != 0)
		return -1;
	return 0;
}

static int fat_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  printf("write path = %s, size=%d, offset=%d\n", path, size, offset);
	meta_t *meta;
	int res;
	int index = get_meta(&fs,path, &meta);
	if (index == -1)
		return -ENOENT;
	//printf("size = %d offset = %d \n", size, offset);
	res = write_data_with_offset(&fs, meta , buf, size, offset);
	write_meta(&fs,meta, index);
	return res;
}

static struct fuse_operations fat_oper = {
        .getattr        = fat_getattr,
        .readdir        = fat_readdir,
        .open           = fat_open,
        .read           = fat_read,
        .opendir        = fat_opendir,
        .mkdir          = fat_mkdir,
        .create         = fat_create,
        .write          = fat_write,
};
int main(int argc, char *argv[])
{
        create_fs(&fs);
        return fuse_main(argc, argv, &fat_oper, NULL);
}
