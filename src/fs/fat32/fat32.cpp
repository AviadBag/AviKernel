#include "fs/fat32/fat32.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "fat32_directory_entry.h"

#include <cstdio.h>
#include <cstdlib.h>

#define FAT32_STORAGE_MAX_SECTORS_READ 255 // How many sectors can you read in every function call?
#define FAT32_DIR_ENTRY_SIZE 32

void FAT32::mount(int drive) 
{
    this->drive = drive;

    // Get the storage driver
    storage_driver = (StorageDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);
    
    // Select our drive in the storage driver
    storage_driver->select_drive(drive);

    // Read the boot sector into my structure
    storage_driver->read_sectors(0, 1, (char*) &boot_sector);

    // Make sure that drive is supported
    if (boot_sector.bytes_per_sector != storage_driver->get_drive(drive)->get_sector_size())
        panic("FAT32 mount drive %d: fs's sector size is not supported!", drive);

    read_fat();
    read_root_dir();
}

void FAT32::read_fat() 
{
    storage_driver->select_drive(drive);
    
    uint32_t fat_lba = boot_sector.reserved_sectors_count;
    uint32_t fat_size_sectors = boot_sector.sectors_per_fat;
    size_t fat_size_bytes = fat_size_sectors * storage_driver->get_drive(drive)->get_sector_size();
    fat = new uint32_t[fat_size_bytes / sizeof(uint32_t)];
    if (!fat)
        panic("FAT32 mount: Not enough memory!");
    // And - read!
    read_sectors_uint32_t(fat_lba, fat_size_sectors, (char*) fat);
}

Vector<uint32_t> FAT32::get_chain(uint32_t cluster) 
{
    Vector<uint32_t> v;
    fat_entry_t fat_entry = cluster;
    do {
        v.append(fat_entry);
        fat_entry = get_fat_entry(fat_entry);        
        if (is_fat_entry_bad(fat_entry))
            panic("FAT32: Encounterd a bad FAT entry");
    } while (!is_fat_entry_last(fat_entry));

    return v;
}

void FAT32::read_root_dir() 
{
    storage_driver->select_drive(drive);

    uint32_t lba = cluster_to_lba(boot_sector.root_cluster);
    fat_entry_t fat_entry = boot_sector.root_cluster;
    uint32_t root_dir_clusters = 0; // How many cluster does the root dir have?
    const uint32_t sector_size_bytes = storage_driver->get_drive(drive)->get_sector_size();
    const size_t cluster_size_bytes = boot_sector.sectors_per_cluster * sector_size_bytes;
    char* ptr; // Holds the current buffer inside of the big root dir buffer that we are now writing into.

    do
    {
        root_dir_clusters++;

        // Allocate space for that cluster
        size_t size_required = cluster_size_bytes * root_dir_clusters;
        if (!root_dir)
        {
            root_dir = new uint8_t[size_required];
            ptr = (char*) root_dir;
        }
        else
        {
            root_dir = (uint8_t*) realloc(root_dir, size_required);
            ptr = (char*) root_dir;
            // Go to the beginning of the space dedicated for that cluster in the root dir
            ptr += cluster_size_bytes * (root_dir_clusters - 1);
        }
        if (!root_dir)
            panic("FAT32 mount: Not enough memory!");

        // Read!
        storage_driver->read_sectors(lba, boot_sector.sectors_per_cluster, ptr);

        fat_entry = get_fat_entry(fat_entry);
        lba = cluster_to_lba(fat_entry);
        
        if (is_fat_entry_bad(fat_entry))
            panic("FAT32: Encounterd a bad FAT entry");
    } while (!is_fat_entry_last(fat_entry));
    
}

void FAT32::read_sectors_uint32_t(uint64_t lba, uint32_t count, char* buffer) 
{
    storage_driver->select_drive(drive);
    if (count <= FAT32_STORAGE_MAX_SECTORS_READ) // We can read right away
        storage_driver->read_sectors(lba, count, buffer);
    else
    {
        int times = count / FAT32_STORAGE_MAX_SECTORS_READ;
        for (int i = 0; i < times; i++)
        {
            storage_driver->read_sectors(lba, FAT32_STORAGE_MAX_SECTORS_READ, buffer);
            buffer += FAT32_STORAGE_MAX_SECTORS_READ * storage_driver->get_drive(drive)->get_sector_size();
            lba += FAT32_STORAGE_MAX_SECTORS_READ;
        }
        storage_driver->read_sectors(lba, times % FAT32_STORAGE_MAX_SECTORS_READ, buffer);
    }
}

uint32_t FAT32::cluster_to_lba(uint32_t cluster) 
{
    // First, where does the data region begin? (In LBA!)
    // Spoiler: It starts right after the reserved sectors and the FATs.
    uint32_t data_begin_lba = boot_sector.reserved_sectors_count + (boot_sector.sectors_per_fat * boot_sector.number_of_fats);
    
    // The first cluster is cluster 2.
    cluster -= 2;

    // Well, pretty simple.
    return data_begin_lba + (cluster * boot_sector.sectors_per_cluster);
}

void FAT32::umount() 
{

}

fat_entry_t FAT32::get_fat_entry(uint32_t cluster) 
{
    return fat[cluster] & 0x0FFFFFFF; // The high 4 bits should be ignored!
}

bool FAT32::is_fat_entry_last(fat_entry_t entry) 
{
    return entry > 0x0FFFFFF8;
}

bool FAT32::is_fat_entry_bad(fat_entry_t entry) 
{
    return entry == 0x0FFFFFF7;
}

void FAT32::read([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{
    uint8_t* current_entry = root_dir;
    while (true)
    {
        FAT32DirectoryEntry entry(current_entry);

        if (entry.is_end()) break;

        if (!entry.is_long_entry())
        {
            // Get the FAT entry of this file
            fat_entry_t fat_entry = get_fat_entry(entry.se_get_first_cluster());
            if (is_fat_entry_bad(fat_entry))
                panic("FAT32: Encountered a bad FAT entry\n");
            printf("File Name: %s", entry.se_get_name().c_str());
            if (is_fat_entry_last(fat_entry))
                printf(", it's only cluster is %u\n", entry.se_get_first_cluster());
            else
                printf(", it's first cluster is %u, and the second is %u\n", entry.se_get_first_cluster(), fat_entry);
        }

        current_entry += FAT32_DIR_ENTRY_SIZE;
    }
}

void FAT32::write([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{

}

void FAT32::append([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{

}
