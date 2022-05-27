#include "fs/ext2/ext2.h"
#include "fs/vfs/vfs.h"
#include "fs/path.h"

#include <cstdio.h>

int Ext2::mount(Path what)
{
    printf("Mounting Ext2...");

    // Read superblock
}

int Ext2::umount()
{
}

uint64_t Ext2::read(Path path, uint64_t count, uint64_t offset, char *buf)
{
}

uint64_t Ext2::write(Path path, uint64_t count, uint64_t offset, char *buf)
{
}

uint64_t Ext2::get_file_size(Path path, uint64_t *size)
{
}

uint64_t Ext2::create_file(Path path)
{
}

uint64_t Ext2::delete_file(Path path)
{
}

uint64_t Ext2::list_files(Path path, Vector<Path> *)
{
}

int Ext2::file_exist(Path path)
{
}
