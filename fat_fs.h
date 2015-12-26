#define BLOCK_NUMBER 1024
#define BLOCK_SIZE 2048
#define FILENAME_LENGTH 100
#define FILE_NUMBER 64
#define BLOCK_FREE -2
#define BLOCK_EOF -1
#define FS_FILE "/home/dmitriy/Desktop/ostasks/fat_fs/fs_file"
#ifndef FAT_FS_H
#define FAT_FS_H
struct meta {
	char busy;
	char is_folder;
	int start_block;
	int block_size;
	char name[FILENAME_LENGTH];
};
typedef struct meta meta_t;

struct fs {
	int next[BLOCK_NUMBER];
	meta_t meta[FILE_NUMBER];
};
typedef struct fs fs_t;
#endif
//write data writes only one block, need to repair write_precedence_vector 
