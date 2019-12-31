// based on cs3650 starter code

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include "inode.h"
#include "storage.h"
#include "super_block.h"
#include "data_block.h"


// implementation for: man 2 access
// Checks if a file exists.
int
nufs_access(const char *path, int mask)
{
    int rv = 0;
    printf("access(%s, %04o) -> %d\n", path, mask, rv);
    return rv;
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int
nufs_getattr(const char *path, struct stat *st)
{
	int rv = 0;
	rv = storage_stat(path, st);
    if (rv != -1) {
        return 0;
    }
   
	return -ENOENT;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int
nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    struct stat st;
    int rv;

	storage_stat("/", &st);
    rv = filler(buf, ".", &st, 0);
	assert(rv == 0);

    int num_nodes = get_inode_count();
	void* bitmap = get_inode_bitmap_addr();
	void* inode_addr = get_inode_addr();

	for (int i = 0; i < num_nodes; ++i)
	{
		int bit = *((int*) bitmap + (i * sizeof(int)));
		if (bit == 1)
		{
			void* ptr = ((void*) inode_addr + (i * sizeof(inode)));
			inode* node = ((inode*) ptr);

			if (!(strcmp(node->path, "/") == 0))
			{
				storage_stat(node->path, &st);
				filler(buf, node->name, &st, 0);
			}
		}
	}
	
    printf("readdir(%s) -> %d\n", path, rv);
    return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int
nufs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    printf("mknod(%s, %04o)\n", path, mode);
    int idx = get_empty_inode();
    if (idx != -1) 
	{
        set_inode_bitmap(idx, 1);
		create_inode(path, mode, idx);
    }
	else 
	{
        return -1;
	}
    return 0;  
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int
nufs_mkdir(const char *path, mode_t mode)
{
    int rv = get_empty_inode();

	if (rv == -1)
	{
		return rv;
	}
	set_inode_bitmap(1, rv);
	create_inode(path, S_IFDIR|S_IRWXU, rv);
    printf("mkdir(%s) -> %d\n", path, rv);
    return rv;
}

int
nufs_unlink(const char *path)
{
	inode* node = get_file(path);

    set_inode_bitmap(0, node->id);
    set_block_bitmap(0, node->datablock_id);

    return 0;
}

int
nufs_link(const char *from, const char *to)
{
	inode* from_node = get_file(from);

  	nufs_mknod(to, S_IFREG, 0);
  	struct stat st;
  	nufs_getattr(to, &st);

  	inode* to_node = get_file(to);

 	to_node->size = from_node->size;
 	to_node->datablock_id = from_node->datablock_id;

  	return 0; 
}

int
nufs_rmdir(const char *path)
{
    int rv = -1;
    printf("rmdir(%s) -> %d\n", path, rv);
    return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int
nufs_rename(const char *from, const char *to)
{
    printf("rename(%s => %s)\n", from, to);
    inode* node = get_file(from);
    rename_inode(node, to);
    return 0;
}

int
nufs_chmod(const char *path, mode_t mode)
{
    int rv = -1;
    printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

int
nufs_truncate(const char *path, off_t size)
{
    int rv = -1;
    printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
    return rv;
}

// this is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
int
nufs_open(const char *path, struct fuse_file_info *fi)
{
    int rv = 0;
    printf("open(%s) -> %d\n", path, rv);
    return rv;
}

// Actually read data
int
nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("read(%s, %ld bytes, @%ld)\n", path, size, offset);
	int new_size = 1;
    const char* data = get_data(path);

    new_size += strlen(data) + 1;
    if (size < new_size) {
        new_size = size;
    }
    strlcpy(buf, data, new_size);
    return new_size;
}

// Actually write data
int
nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    inode* node = get_file(path);
    if (!node) {
        return -1;  
    }
    assert(node->datablock_id != -1);
    void* ptr = get_ptr_at_idx(node->datablock_id);
    memcpy(ptr, buf, size);
    node->size = size;
    update_block_size(size, node->datablock_id);

    return 0;
}

// Update the timestamps on a file or directory.
int
nufs_utimens(const char* path, const struct timespec ts[2])
{
    int rv = -1;
    printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n",
           path, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
	return rv;
}

// Extended operations
int
nufs_ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi,
           unsigned int flags, void* data)
{
    int rv = -1;
    printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
    return rv;
}

void
nufs_init_ops(struct fuse_operations* ops)
{
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access   = nufs_access;
    ops->getattr  = nufs_getattr;
    ops->readdir  = nufs_readdir;
    ops->mknod    = nufs_mknod;
    ops->mkdir    = nufs_mkdir;
    ops->link     = nufs_link;
    ops->unlink   = nufs_unlink;
    ops->rmdir    = nufs_rmdir;
    ops->rename   = nufs_rename;
    ops->chmod    = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open	  = nufs_open;
    ops->read     = nufs_read;
    ops->write    = nufs_write;
    ops->utimens  = nufs_utimens;
    ops->ioctl    = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int
main(int argc, char *argv[])
{
    assert(argc > 2 && argc < 6);
    storage_init(argv[--argc]);
    nufs_init_ops(&nufs_ops);
    return fuse_main(argc, argv, &nufs_ops, NULL);
}

