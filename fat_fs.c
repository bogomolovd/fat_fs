#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define BLOCK_NUMBER 1024
#define BLOCK_SIZE 2048
#define FILENAME_LENGTH 100
#define FILE_NUMBER 64
#define BLOCK_FREE -2
#define BLOCK_EOF -1
#define FS_FILE "/home/dmitriy/projects/fat_fs/fs_file"

typedef struct meta {
	char busy;
	char is_folder;
	int start_block;
	int block_size;
	char name[FILENAME_LENGTH];
} meta_t;

//block is free: next[i] == -2
//block points to another block: next[i] == n
//block contains end of file: next[i] == -1

typedef struct fs {
	int next[BLOCK_NUMBER];
	meta_t fd[FILE_NUMBER];
} fs_t;

fs_t fs;

static int fat_getattr(const char *path, struct stat *stbuf)
{
        printf("getattr path = %s\n", path);
        return 0;
}
static int fat_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
        printf("readdir path = %s\n", path);
        return 0;
}
static int fat_open(const char *path, struct fuse_file_info *fi)
{
        printf("opendir path = %s\n", path);
        return 0;
}
static int fat_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
        printf("read path = %s\n", path);
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
        return fuse_main(argc, argv, &fat_oper, NULL);
}
