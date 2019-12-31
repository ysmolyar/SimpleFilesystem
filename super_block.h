// based on cs3650 starter code

#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>
#include "inode.h"

void superblock_init(const char* path);
int get_datablock_count();
int get_inode_count();
void* get_inode_bitmap_addr();
void* get_datablock_bitmap_addr();
void* get_datablock_addr();
void* get_inode_addr();
void init_superblock_ptrs();
void insert_data_at_offset(int data, int offset);
void init_size_and_count() ;


#endif
