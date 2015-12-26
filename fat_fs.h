#define BLOCK_NUMBER 1024
#define BLOCK_SIZE 2048

#define FILENAME_LENGTH 100
#define FILE_NUMBER 64

#define BLOCK_FREE 0
#define BLOCK_EOF -1

#define ERR_NFM 1000
#define ERR_NFB 1001

#define FS_FILE "/home/dmitriy/Projects/fat_fs/fs_file"

#ifndef FAT_FS_H
#define FAT_FS_H
struct meta {
	char busy;
	char is_folder;
	int start_block;
	int file_size;
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


#define BUFFER_SIZE 16

int create_fs(fs_t *fs);
int init_fs(fs_t *fs);
int create_empty_file(fs_t *fs, char *name, char is_folder);
