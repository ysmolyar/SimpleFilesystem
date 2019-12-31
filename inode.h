// based on cs3650 starter code

#ifndef INODE_H
#define INODE_H

#include "super_block.h"

typedef struct inode {
	char path[32]; // path to file
	char name[32]; // file name
    int datablock_id; // id of datablock
	int id; //id of inode
    int mode; // permission & type
    int size; // bytes
} inode;

void print_inode(inode *node);
void create_inode(const char* path, int mode, int id);
void set_inode_bitmap(int id, int bit);
int get_empty_inode();
int get_terminal_delimiter_idx(const char* path);
const char* get_name(const char* path);
void rename_inode(inode *node, const char *path);

#endif
