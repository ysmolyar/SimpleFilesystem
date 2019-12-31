#include "data_block.h"
#include "inode.h"
#include "storage.h"
#include <string.h>

void 
print_inode(inode *node)
{
    if (node) 
	{
        printf("node: {path: %s, name: %s, datablock_id: %d, id: %d, 			mode: %d, size: %d}\n", node->path, node->name, 
		node->datablock_id, node->id, node->mode, node->size);
    } 
	else 
	{
        printf("node is null\n");
    }
}

void 
create_inode(const char* path, int mode, int id)
{

	void* inode_ptr = get_inode_addr();
    void* addr = inode_ptr + (sizeof(inode) * id);

    inode *new_node = (inode *) (addr);
    new_node->mode = mode;
    rename_inode(new_node, path);

	int num_blocks = get_datablock_count();
	void* bitmap = get_inode_bitmap_addr();
    int block_id = -1;
    for (int i = 0; i < num_blocks; i++) 
	{
        if (*((int *) ((sizeof(int) * i) + bitmap)) != 1) 
		{
            block_id = i;
            break;
        }
    }

    if (block_id == -1) 
	{
        new_node->datablock_id = -1;
    } 
	else 
	{
		set_block_bitmap(1, block_id);
        new_node->datablock_id = block_id;
    }
    new_node->size = 0;
    new_node->id = id;
}

void set_inode_bitmap(int id, int bit)
{
	void* bitmap = get_inode_bitmap_addr();
    *((int *) bitmap + (id * sizeof(int))) = bit;
}

int get_empty_inode()
{
	int num_nodes = get_inode_count();
	void* bitmap = get_inode_bitmap_addr();
    for (int i = 0; i < num_nodes; ++i) {
        if (*((int *) (bitmap + (i * sizeof(int)))) != 1) {
            return i;
        }
    }
    return -1;
}

int get_terminal_delimiter_idx(const char* path)
{

    int idx = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (strcmp((const char *) (((void *) path) + i), "/") == 0) {
            idx = i;
        }
    }
	return idx;
}

void rename_inode(inode *node, const char *path)
{
    for (int i = 0; i < strlen(path); i++) {
        node->path[i] = path[i];
    }
    node->path[strlen(path)] = NULL;
	int count = 0;
	int start = get_terminal_delimiter_idx(path) + 1;
    for (int i=start;i<strlen(path);i++) {
        node->name[count] = path[i];
		count++;
    }
    node->name[count] = NULL;
}
