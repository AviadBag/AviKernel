#include "fs/ext2/ext2.h"
#include "fs/vfs/vfs.h"
#include "fs/path.h"

#include <posix/errno.h>
#include <cstdio.h>
#include <cstring.h>

#define EXT2_SUPERBLOCK_OFFSET 1024 // Starting 1024 bytes from beginning of disk
#define EXT2_FIRST_INDIRECT_BLOCK 13

int Ext2::mount(Path what)
{
    printf("Mounting Ext2...\n");

    // Acquire file descriptor for the disk
    disk = VFS::get_instance()->open(what.to_string().c_str(), O_RDWR);
    if (!disk)
        return false;

    // Read superblock
    if (!VFS::get_instance()->pread(disk, &super_block, sizeof(ext2_super_block), EXT2_SUPERBLOCK_OFFSET))
        return false;

    printf("Block size: %llu KiB\n", get_block_size() / 1024);

    // Read block groups table
    if (!read_block_groups_table())
        return false;

    // Read root directory
    printf("Reading root directory... (inode %d)\n", EXT2_ROOT_INO);
    if (!read_inode_struct(EXT2_ROOT_INO, &root_directory))
        return false;

    print_inode(root_directory);

    // For testing - read lesmiserables.txt - inode 0x10
    ext2_inode lames_inode;
    read_inode_struct(0x10, &lames_inode);
    char *buf = new char[8000];
    read_inode(lames_inode, buf, 8000, 0);
    for (int i = 0; i < 256; i++)
        putchar(buf[i]);
    for (int i = 7000; i < 8000; i++)
        putchar(buf[i]);

    return true;
}

void Ext2::print_inode(ext2_inode inode)
{
    bool is_dir = inode.i_mode & EXT2_S_IFDIR;
    printf("Inode type: %s", is_dir ? "Directory" : "File");
    if (is_dir)
        printf(", Is a hashed index directory? %s", inode.i_flags & EXT2_INDEX_FL ? "Yes" : "No");

    putchar('\n');
}

bool Ext2::read_inode(ext2_inode inode, void *buf, uint64_t count, uint64_t offset)
{
    if (offset != 0)
        panic("EXT2: read_inode with offset is not yet implemented");

    // How many blocks do we need to read?
    uint64_t number_of_blocks = count / get_block_size() + 1;
    if (number_of_blocks >= EXT2_FIRST_INDIRECT_BLOCK)
        panic("EXT2: reading indirect blocks is not yet implemented");

    // For every block
    for (int i = 0; i < number_of_blocks; i++)
    {
        // Read!
        block_t block = inode.i_block[i];

        bool last_block = (i + 1 == number_of_blocks);
        uint64_t read_size = (last_block ? count : get_block_size());
        if (!read_block(block, buf, read_size))
            return false;
        count -= read_size;
        buf += read_size;
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
    block_t group_descs_table_block = get_block_size() == 1024 ? 2 : 1;
    return VFS::get_instance()->pread(
        disk,
        group_desc_table,
        blocks_gropus_count * sizeof(ext2_group_desc),
        get_block_offset(group_descs_table_block));
}

uint64_t Ext2::get_block_offset(block_t block)
{
    return block * get_block_size();
}

bool Ext2::read_block(block_t block, void *buf, uint64_t count = 0)
{
    return VFS::get_instance()->pread(disk, buf, count == 0 ? get_block_size() : count, get_block_offset(block));
}

bool Ext2::read_inode_struct(inode_t inode, ext2_inode *buf)
{
    /**
     * IMPORTANT COMMENT
     * sizeof(ext2_inode) is 128, but super_block.s_inode_size might be 256, or even more.
     * Therefore, when copying inode into the buffer, you must copy the first 128 bytes only.
     * But when iterating over inodes, the jump should be of super_block.s_inode_size
     */

    // In what block group our inode is?
    uint64_t block_group = (inode - 1) / super_block.s_inodes_per_group;

    // What's our index in the block group's inode table?
    uint64_t index_inode_table = (inode - 1) % super_block.s_inodes_per_group;

    // How many inodes per block in the inode table?
    uint64_t inodes_per_block = get_block_size() / super_block.s_inode_size;

    // What block do we need?
    block_t block = group_desc_table[block_group].bg_inode_table + (index_inode_table / inodes_per_block);

    // Read the block
    char *block_buf = new char[get_block_size()];
    if (!block_buf)
    {
        set_errno(ENOMEM);
        return false;
    }
    if (!read_block(block, block_buf))
        return false;

    // What inode is it in the block?
    uint64_t inode_in_block = index_inode_table % inodes_per_block;

    // Copy the correct inode from the block
    memcpy(buf, block_buf + (inode_in_block * super_block.s_inode_size), sizeof(ext2_inode));

    delete block_buf;
    return true;
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
