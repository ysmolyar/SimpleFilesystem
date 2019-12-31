#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "super_block.h"

int get_empty_block();
void* get_ptr_at_idx(int idx);
void update_block_size(int newSize, int block_idx);
void set_block_bitmap(int bit, int block_idx);

#endif