#include "fs/fat32/fat32.h"

#include <stdint.h>

// Struct definitions
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

void mount(int what)
{
	// TODO: Make sure that the bootsector struct is really 512 bytes size!!
}

void umount()
{

}

void read(char* path, int count, char buf)
{

}

void write(char* path, int count, char buf)
{

}

void append(char* path, int count, char buf)
{

}
