#include "fs/devfs/devfs.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/storage/physical_drive.h"

#include <cstdio.h>

/*
Mapping:
    /sda - first drive
    /sdb - second drive
    ...
*/

void DevFS::mount([[gnu::unused]] int what) 
{
    
}

void DevFS::umount() {} // Nothing here

fs_status_code DevFS::read(Path path, size_t count, size_t offset, char* buf) 
{

}

void DevFS::read_storage_device(String file_name, size_t count, size_t offset, char* buf) 
{
    
}

fs_status_code DevFS::write(Path path, size_t count, size_t offset, char* buf) 
{

}

fs_status_code DevFS::create_file(Path path) 
{

}

fs_status_code DevFS::delete_file(Path path) 
{

}

fs_status_code DevFS::list_files(Path path, Vector<Path>*) 
{

}
