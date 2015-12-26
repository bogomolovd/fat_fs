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
      //  printf("readdir path = %s\n", path);
        return 0;
}

static int fat_open(const char *path, struct fuse_file_info *fi)
{
      //  printf("opendir path = %s\n", path);
        return 0;
}

static int fat_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
      //  printf("read path = %s\n", path);
        return size;
}

static struct fuse_operations fat_oper = {
        .getattr        = fat_getattr,
        .readdir        = fat_readdir,
        .open           = fat_open,
        .read           = fat_read,
};
int main(int argc, char *argv[])
{
        create_fs(&fs);
        return fuse_main(argc, argv, &fat_oper, NULL);
}
