#ifndef _FAT32_H
#define _FAT32_H

#include "fs/fs.h"

#include <stdint.h>

// This is a read-only implementation of the FAT 32 file system

struct fat32_boot_sector
{
	uint8_t unused1[3];
	char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors_count;
	uint8_t number_of_fats;
	uint16_t unused2;
	uint16_t unused3;
	uint8_t media;
	uint16_t unused4;
	uint16_t sectors_per_track;
	uint16_t number_of_heads;
	uint32_t number_of_hidden_sectors;
	uint32_t number_of_sectors;

	uint32_t sectors_per_fat;
	uint16_t extra_flags;
	uint16_t fs_version;
	uint32_t root_cluster;
	uint16_t fs_info_sector;
	uint16_t boot_sector_copy_sector;
	uint8_t reserved1[12];
	uint8_t unused5;
	uint8_t reserved2;
	uint8_t boot_sig;
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
	char boot_code[420];
	uint16_t bootable_signature;
} __attribute__((__packed__));

using fat_entry_t = uint32_t;
using cluster_number = uint64_t;
using sector_number = uint64_t;

class FAT32 : public FS
{
public:
	// ------------------- Methods Overriden -------------------
	virtual void mount(Path what) override;
	virtual void umount() override;

	virtual uint64_t read(Path path, uint64_t count, uint64_t offset, char *buf) override;
	virtual uint64_t write(Path path, uint64_t count, uint64_t offset, char *buf) override;

	virtual uint64_t get_file_size(Path path, uint64_t *size) override;
	virtual uint64_t create_file(Path path) override;
	virtual uint64_t delete_file(Path path) override;
	virtual uint64_t list_files(Path path, Vector<Path> *) override;

	virtual bool file_exist(Path path) override;

private:
	// Converts the given sector number to bytes offset that can be given to pread
	uint64_t sector_to_offset(sector_number sector);

	// Returns the next cluster in the chain.
	cluster_number get_next_cluster(cluster_number);

	// Is it the last cluster in the chain?
	bool is_last_cluster(cluster_number);

	int raw_disk;				   // A file pointer to the raw disk
	fat32_boot_sector boot_sector; // Will hold the boot sector info.
	/**
	 * Will point to the FAT. I can't know now it's size, for it depends on boot_sector.sectors_per_fat.
	 */
	uint32_t *fat;
	uint64_t fat_size_bytes;
};

#endif
