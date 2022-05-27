#ifndef _EXT2_H
#define _EXT2_H

#include "fs/fs.h"
#include "fs/path.h"

#include <stdint.h>

struct ext2_super_block
{
    uint32_t s_inodes_count;           /* Inodes count */
    uint32_t s_blocks_count;           /* Blocks count */
    uint32_t s_r_blocks_count;         /* Reserved blocks count */
    uint32_t s_free_blocks_count;      /* Free blocks count */
    uint32_t s_free_inodes_count;      /* Free inodes count */
    uint32_t s_first_data_block;       /* First Data Block */
    uint32_t s_log_block_size;         /* Block size */
    uint32_t s_log_frag_size;          /* Fragment size */
    uint32_t s_blocks_per_group;       /* # Blocks per group */
    uint32_t s_frags_per_group;        /* # Fragments per group */
    uint32_t s_inodes_per_group;       /* # Inodes per group */
    uint32_t s_mtime;                  /* Mount time */
    uint32_t s_wtime;                  /* Write time */
    uint16_t s_mnt_count;              /* Mount count */
    uint16_t s_max_mnt_count;          /* Maximal mount count */
    uint16_t s_magic;                  /* Magic signature */
    uint16_t s_state;                  /* File system state */
    uint16_t s_errors;                 /* Behaviour when detecting errors */
    uint16_t s_minor_rev_level;        /* minor revision level */
    uint32_t s_lastcheck;              /* time of last check */
    uint32_t s_checkinterval;          /* max. time between checks */
    uint32_t s_creator_os;             /* OS */
    uint32_t s_rev_level;              /* Revision level */
    uint16_t s_def_resuid;             /* Default uid for reserved blocks */
    uint16_t s_def_resgid;             /* Default gid for reserved blocks */
    uint32_t s_first_ino;              /* First non-reserved inode */
    uint16_t s_inode_size;             /* size of inode structure */
    uint16_t s_block_group_nr;         /* block group # of this superblock */
    uint32_t s_feature_compat;         /* compatible feature set */
    uint32_t s_feature_incompat;       /* incompatible feature set */
    uint32_t s_feature_ro_compat;      /* readonly-compatible feature set */
    uint8_t s_uuid[16];                /* 128-bit uuid for volume */
    char s_volume_name[16];            /* volume name */
    char s_last_mounted[64];           /* directory where last mounted */
    uint32_t s_algorithm_usage_bitmap; /* For compression */
    uint8_t s_prealloc_blocks;         /* Nr of blocks to try to preallocate*/
    uint8_t s_prealloc_dir_blocks;     /* Nr to preallocate for dirs */
    uint16_t s_padding1;               /* */
    uint8_t s_journal_uuid[16];        /* uuid of journal superblock */
    uint32_t s_journal_inum;           /* inode number of journal file */
    uint32_t s_journal_dev;            /* device number of journal file */
    uint32_t s_last_orphan;            /* start of list of inodes to delete */
    uint32_t s_hash_seed[4];           /* HTREE hash seed */
    uint8_t s_def_hash_version;        /* Default hash version to use */
    uint8_t s_reserved_char_pad;       /* */
    uint16_t s_reserved_word_pad;      /* */
    uint32_t s_default_mount_opts;     /* */
    uint32_t s_first_meta_bg;          /* First metablock block group */
    uint32_t s_reserved[190];          /* Padding to the end of the block */
};

class Ext2 : public FS
{
public:
    // ------------------- Methods Overriden -------------------
    virtual int mount(Path what) override;
    virtual int umount() override;

    virtual uint64_t read(Path path, uint64_t count, uint64_t offset, char *buf) override;
    virtual uint64_t write(Path path, uint64_t count, uint64_t offset, char *buf) override;

    virtual uint64_t get_file_size(Path path, uint64_t *size) override;
    virtual uint64_t create_file(Path path) override;
    virtual uint64_t delete_file(Path path) override;
    virtual uint64_t list_files(Path path, Vector<Path> *) override;

    virtual int file_exist(Path path) override;
};

#endif