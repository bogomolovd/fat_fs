#include "fat_fs.h"
#define FIRST_BLOCK 1

int find_free_meta_index(fs_t *fs);
meta_t* get_meta_by_index(fs_t *fs, int index);
int find_free_block(fs_t *fs);
void write_meta(fs_t *fs, meta_t *meta, int index);
void write_precedence_vector(fs_t *fs, meta_t *meta);
int get_meta(fs_t *fs, const char *path, meta_t **meta);
int get_data(fs_t *fs, meta_t *meta, char **buffer);
int get_index(fs_t *fs, char *data, int size, char *filename);
char* get_directory(const char *path);
char* get_filename(const char *path);
int write_data(fs_t *fs, meta_t *meta, void *data, int size);
