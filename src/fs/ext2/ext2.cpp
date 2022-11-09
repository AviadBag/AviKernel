#include "fs/ext2/ext2.h"
#include "fs/vfs/vfs.h"
#include "fs/path.h"

#include "utils/io.h"
#include "utils/macro.h"

#include <posix/errno.h>
#include <cstdio.h>
#include <cstring.h>

#define EXT2_SUPERBLOCK_OFFSET 1024  // Starting 1024 bytes from beginning of disk
#define EXT2_FIRST_INDIRECT_BLOCK 12 // Starting from index 0

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
    uint64_t size = 30000;
    char *buf = new char[size];
    if (!buf)
        panic("Cannot read test file lames - out of memory");
    read_inode(lames_inode, buf, size, 0);
    for (uint64_t i = 0; i < size; i++)
    {
        putchar(buf[i]);
        IO::outb(0x3F8, buf[i]);
    }
    printf("Done\n");

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
    uint32_t no_blocks = calc_no_blocks(count, offset);
    block_num *blocks_nums = new block_num[no_blocks]; // Will contain the list of blocks we need to read
    if (!get_blocks_nums(inode, blocks_nums, no_blocks, offset))
        return false;

    // Read every block
    for (uint32_t i = 0; i < no_blocks; i++)
    {
        uint64_t offset_in_block = 0; // Offset in current block
        if (i == 0)                   // First block
            offset_in_block = offset % get_block_size();

        // How many to read from the block
        uint64_t count_in_block = get_block_size() - offset_in_block;
        if (i == no_blocks - 1) // Last block
            count_in_block = (offset + count) % get_block_size();

        // Read!
        if (!read_block(blocks_nums[i], buf, count_in_block, offset_in_block))
            return false;

        ADD_TO_VOID_P(buf, count_in_block);
    }

    return true;
}

bool Ext2::get_blocks_nums(ext2_inode inode, block_num *array, uint32_t no_blocks, uint64_t offset)
{
    // Index of first block in blocks array
    uint32_t first_block_idx = offset / get_block_size();
    uint32_t last_block_idx = first_block_idx + no_blocks - 1;

    // Simplest case - we do not need to read indirect blocks
    if (last_block_idx < EXT2_FIRST_INDIRECT_BLOCK)
    {
        memcpy(array, &inode.i_block[first_block_idx], no_blocks * sizeof(block_num));
        return true;
    }

    set_errno(ENOTSUP);
    return false;
}

uint32_t Ext2::calc_no_blocks(uint64_t bytes_count, uint64_t offset)
{
    if (bytes_count == 0)
        return 0;

    // We surely need to read the first block
    uint32_t blocks_count = 1;

    uint64_t offset_in_1st_block = offset % get_block_size();
    bytes_count -= (get_block_size() - offset_in_1st_block);

    // Now offset is aligned to block size
    blocks_count += (bytes_count / get_block_size());

    // Check block align overflow
    if (bytes_count % get_block_size() != 0)
        blocks_count++;

    return blocks_count;
}

uint64_t Ext2::get_inode_size(ext2_inode inode)
{
    /**
     * In revision 0, i_dir_acl is 0, and i_size is the size.
     * In revision 1, i_dir_acl is the higher 32 bits, and i_size is the lower 32 bits.
     */

    uint64_t size = 0;
    size |= inode.i_size;
    size |= ((uint64_t)inode.i_dir_acl << 32);

    return size;
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
    block_num group_descs_table_block = get_block_size() == 1024 ? 2 : 1;
    return VFS::get_instance()->pread(
        disk,
        group_desc_table,
        blocks_gropus_count * sizeof(ext2_group_desc),
        get_block_offset(group_descs_table_block));
}

uint64_t Ext2::get_block_offset(block_num block)
{
    return block * get_block_size();
}

bool Ext2::read_block(block_num block, void *buf, uint64_t count = 0, uint64_t offset = 0)
{
    return VFS::get_instance()->pread(disk, buf, count == 0 ? get_block_size() : count, get_block_offset(block) + offset);
}

bool Ext2::read_inode_struct(inode_num inode, ext2_inode *buf)
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
    block_num block = group_desc_table[block_group].bg_inode_table + (index_inode_table / inodes_per_block);

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

uint64_t Ext2::read([[gnu::unused]] Path path, [[gnu::unused]] uint64_t count, [[gnu::unused]] uint64_t offset, [[gnu::unused]] char *buf)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::write([[gnu::unused]] Path path, [[gnu::unused]] uint64_t count, [[gnu::unused]] uint64_t offset, [[gnu::unused]] char *buf)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::get_file_size([[gnu::unused]] Path path, [[gnu::unused]] uint64_t *size)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::create_file([[gnu::unused]] Path path)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::delete_file([[gnu::unused]] Path path)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::list_files([[gnu::unused]] Path path, [[gnu::unused]] Vector<Path> *)
{
    set_errno(ENOTSUP);
    return 0;
}

int Ext2::file_exist([[gnu::unused]] Path path)
{
    set_errno(ENOTSUP);
    return 0;
}

uint64_t Ext2::get_block_size()
{
    return 1024 << super_block.s_log_block_size;
}

uint64_t Ext2::get_blocks_gropus_count()
{
    return (super_block.s_blocks_count + (super_block.s_blocks_per_group - 1)) / super_block.s_blocks_per_group;
}
