#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "storage.h"

#include "data_block.h"

void
storage_init(const char* path) {
	superblock_init(path);
}

int    
storage_stat(const char* path, struct stat* st)
{
   inode *data = get_file(path);
    if (!data) {
        return -1;
    }

    memset(st, 0, sizeof(struct stat));
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_mode = data->mode;
    st->st_size = data->size;
    st->st_nlink = 1;

    return 0;
}

inode*
get_file(const char *path) 
{

	int num_nodes = get_inode_count();
	void* bitmap = get_inode_bitmap_addr();
	void* node = get_inode_addr();
    for (int i = 0; i < num_nodes; i++) {

        if (*((int *) (bitmap + sizeof(int) * i)) == 1) 
		{
			void* ptr = ((void *) (node + sizeof(inode) * i));
        	inode* this_node = ((inode *) ptr);

        	if (strcmp(path, this_node->path) == 0) 
			{ 
            	return this_node;
        	}
        } 
    }
    return 0;
}


const char *
get_data(const char *path) {
    inode* node = get_file(path);
    if (!node) {
        return 0;
    }
    int datablock_id = node->datablock_id;
    if (datablock_id == -1) {
        return 0;
    }
    void* ptr = get_ptr_at_idx(datablock_id);
    if (!ptr) {
        return 0;
    }
    return ((const char *) ptr);
}
