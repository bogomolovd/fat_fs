#include "fat_fs.h"
#define FIRST_BLOCK 1

int find_free_meta_index(fs_t *fs);
meta_t* get_meta_by_index(fs_t *fs, int index);
int find_free_block(fs_t *fs);
void write_meta(fs_t *fs, meta_t *meta, int index);
void write_precedence_vector(fs_t *fs, meta_t *meta);
