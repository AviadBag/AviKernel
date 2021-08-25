#ifndef _FAT32_H
#define _FAT32_H

#include "fs/fs.h"
#include "drivers_manager/drivers/storage/storage_driver.h"

#include <stdint.h>

// This is an implementation of the FAT 32 file system


struct fat32_boot_sector
{
	uint8_t  unused1[3];
	char     oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t  sectors_per_cluster;
	uint16_t reserved_sectors_count;
	uint8_t  number_of_fats;
	uint16_t unused2;
	uint16_t unused3;
	uint8_t  media;
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
	uint8_t  reserved1[12];
	uint8_t  unused5;
	uint8_t  reserved2;
	uint8_t  boot_sig;
	uint32_t volume_id;
	char     volume_label[11];
	char     fs_type[8];
	char     boot_code[420];
	uint16_t bootable_signature;
} __attribute__((__packed__));

using fat_entry_t = uint32_t;

class FAT32 : public FS
{
public:
	virtual void mount(int drive) override; 
	virtual void umount() override;

	virtual void read  (char* path, int count, char* buf) override;
	virtual void write (char* path, int count, char* buf) override;
	virtual void append(char* path, int count, char* buf) override;

private:
	void read_fat();
	void read_root_dir();
	// Converts the given cluster to the LBA of it's first sector.
	uint32_t cluster_to_lba(uint32_t cluster);
	// An helper function - reads sectors with count of size 32 bits (instead of 8).
	void read_sectors_uint32_t(uint64_t lba, uint32_t count, char* buffer);
	fat_entry_t get_fat_entry(uint32_t cluster);
	bool is_fat_entry_last(fat_entry_t entry);
	bool is_fat_entry_bad(fat_entry_t entry);

	fat32_boot_sector boot_sector;
	StorageDriver* storage_driver;
	int drive;
	uint32_t* fat = nullptr; // An array, containing the FAT. Will be allocated and filled on mount().
	uint8_t* root_dir = nullptr; // The same..
};

#endif
