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

void DevFS::mount([[gnu::unused]] int what) {} // Nothing here

void DevFS::umount() {} // Nothing here

fs_status_code DevFS::read(Path path, size_t count, size_t offset, char* buf) 
{
    if (path.is_folder())
        panic("DevFS.read() - required file is a folder! (TODO: VFS should check that!)");

    if (path.get_depth() != 0)
        panic("DevFS.read() - file must be on devfs's root directory! (TODO: Just RETURN error)");

    String file_name = path.get_part(0);
    if (file_name.substr(0, 2) == "sd")
        read_storage_device(file_name, count, offset, buf);
    else
        panic("DevFS.read() - file \"%s\" does not exist! (TODO: Just RETURN error)", file_name.c_str());
}

void DevFS::read_storage_device(String file_name, size_t count, size_t offset, char* buf) 
{
    if (file_name.size() != 3)
        panic("DevFS.read() - file \"%s\" does not exist! (TODO: Just RETURN error)", file_name.c_str());
    else if (file_name[2] < 'a' || file_name[2] > 'z')
        panic("DevFS.read() - file \"%s\" does not exist! (TODO: Just RETURN error)", file_name.c_str());

    int drive_number = file_name[2] - 'a'; // <sda> => 0, <sdb> => 1 ....
    StorageDriver* storage_driver = (StorageDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);
    if (drive_number+1 > storage_driver->get_number_of_drives())
        panic("DevFS.read() - file \"%s\" does not exist! (TODO: Just RETURN error)", file_name.c_str());

    // TODO: Better to keap a list of the drives (Fill it on mount), or change the storage driver to RETURN the list.
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
