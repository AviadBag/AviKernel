#include "fs/ext2/ext2.h"
#include "fs/vfs/vfs.h"
#include "fs/path.h"

#include <posix/errno.h>
#include <cstdio.h>

#define SUPERBLOCK_OFFSET 1024 // Starting 1024 bytes from beginning of disk

int Ext2::mount(Path what)
{
    printf("Mounting Ext2...\n");

    // Acquire file descriptor for the disk
    disk = VFS::get_instance()->open(what.to_string().c_str(), O_RDWR);
    if (!disk)
        return false;

    // Read superblock
    if (!VFS::get_instance()->pread(disk, &super_block, sizeof(ext2_super_block), SUPERBLOCK_OFFSET))
        return false;

    printf("Block size: %llu KiB\n", get_block_size() / 1024);

    // Read block groups table
    if (!read_block_groups_table())
        return false;

    for (uint64_t i = 0; i < get_blocks_gropus_count(); i++)
    {
        ext2_group_desc desc = group_desc_table[i];
        printf("%u   ", desc.bg_free_inodes_count);
    }

    return true;
}

bool Ext2::read_block_groups_table()
{
    uint64_t blocks_gropus_count = get_blocks_gropus_count();
    group_desc_table = new ext2_group_desc[blocks_gropus_count];
    if (!group_desc_table)
    {
        set_errno(ENOMEM);
        return false;
    }
    uint64_t group_descs_table_block = get_block_size() == 1024 ? 2 : 1;
    return VFS::get_instance()->pread(
        disk,
        group_desc_table,
        blocks_gropus_count * sizeof(ext2_group_desc),
        get_block_offset(group_descs_table_block));
}

uint64_t Ext2::get_block_offset(uint64_t block)
{
    return block * get_block_size();
}

bool Ext2::read_block(uint64_t block, char *buf)
{
    return VFS::get_instance()->pread(disk, buf, get_block_size(), get_block_offset(block));
}

int Ext2::umount()
{
    return VFS::get_instance()->close(disk);
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

uint64_t Ext2::get_block_size()
{
    return 1024 << super_block.s_log_block_size;
}

uint64_t Ext2::get_blocks_gropus_count()
{
    return (super_block.s_blocks_count + (super_block.s_blocks_per_group - 1)) / super_block.s_blocks_per_group;
}
