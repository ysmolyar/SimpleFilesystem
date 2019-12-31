// based on cs3650 starter code

#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "super_block.h"

void   storage_init(const char* path);
int    storage_stat(const char* path, struct stat* st);
const char* get_data(const char *path);
inode* get_file(const char *path);

#endif
