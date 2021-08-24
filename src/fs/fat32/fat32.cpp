#include "fs/fat32/fat32.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"

#include <cstdio.h>

void FAT32::mount(int drive) 
{
    // Get the storaage driver
    storage_driver = (StorageDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);
    
    // Read the boot sector into my structure
    storage_driver->select_drive(drive);
    storage_driver->read_sectors(0, 1, (char*) &boot_sector);
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

void FAT32::read([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{

}

void FAT32::write([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{

}

void FAT32::append([[gnu::unused]] char* path, [[gnu::unused]] int count, [[gnu::unused]] char* buf) 
{

}
