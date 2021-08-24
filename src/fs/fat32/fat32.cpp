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
