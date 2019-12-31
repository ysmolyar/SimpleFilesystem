#include "data_block.h"
#include "super_block.h"

void* get_ptr_at_idx(int idx)
{
	return (4096 * idx) + get_datablock_addr();
}

void update_block_size(int newSize, int block_idx)
{
	int num_nodes = get_inode_count();
	void* bitmap = get_inode_bitmap_addr();
	for (int i = 0; i < num_nodes; ++i)
	{
		int bit = *((int*) bitmap + (i * sizeof(int)));
		if (bit == 1)
		{
			void* inode_ptr = get_inode_addr();
			inode* node = ((inode*) inode_ptr + (i * sizeof(inode)));
			if (node->datablock_id == block_idx)
			{	
				node->size = newSize;
			}
		}
	}
}

void set_block_bitmap(int bit, int block_idx)
{
	void* bitmap = get_inode_bitmap_addr();
	*((int*) (bitmap + (block_idx * sizeof(int)))) = bit;
}