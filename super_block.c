// based on cs3650 starter code

#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "util.h"
#include "super_block.h"
#include "inode.h"

const int PAGE_COUNT = 256;
const int NUFS_SIZE  = 4096 * 256; // 1MB

static int   pages_fd   = -1;
static void* pages_base =  0;

//size of superblock
static int superblock_size;
//number of datablocks
static int datablock_count;
//number of inodes
static int inode_count;
//pointer to first inode
static int inode_ptr;
//pointer to first datablock
static int datablock_ptr;
//pointer to inode bitmap
static int inode_bitmap_ptr;
//pointer to datablock bitmap
static int datablock_bitmap_ptr;

void
superblock_init(const char* path)
{
    pages_fd = open(path, O_CREAT | O_RDWR, 0644);
    assert(pages_fd != -1);

    int rv = ftruncate(pages_fd, NUFS_SIZE);
    assert(rv == 0);

    pages_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pages_fd, 0);
    assert(pages_base != MAP_FAILED);

	//initialize pointers
    init_size_and_count();
	inode_bitmap_ptr = superblock_size;
	datablock_bitmap_ptr = inode_bitmap_ptr + (inode_count * sizeof(int));
	inode_ptr = datablock_bitmap_ptr + (datablock_count * sizeof(int));
	datablock_ptr = inode_ptr + (inode_count * sizeof(inode));
	init_superblock_ptrs();

	create_inode("/", 040755, 0);
    set_inode_bitmap(1, 0);
	
}

int
get_datablock_count()
{
	return datablock_count;
}

int
get_inode_count()
{
	return inode_count;
}

void*
get_inode_bitmap_addr()
{
    return pages_base + inode_bitmap_ptr;
}

void*
get_datablock_bitmap_addr()
{
    return pages_base + datablock_bitmap_ptr;
}

void*
get_datablock_addr()
{
    return pages_base + datablock_ptr;
}

void*
get_inode_addr()
{
    return pages_base + inode_ptr;
}


void
init_superblock_ptrs()
{
	insert_data_at_offset(datablock_ptr, 3);
	insert_data_at_offset(inode_ptr, 2);
	insert_data_at_offset(datablock_bitmap_ptr, 1);	
	insert_data_at_offset(inode_bitmap_ptr, 0);
}

void
insert_data_at_offset(int data, int offset)
{
	int dest = *((int*) (pages_base + (sizeof(int) * offset)));
	dest = data;
}

void
init_size_and_count() 
{
	inode_count = 8;
	datablock_count = 10;
	superblock_size = 20;
}
