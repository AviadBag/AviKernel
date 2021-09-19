#include "fs/devfs/devfs.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/storage/physical_drive.h"

#include <cstdio.h>

void DevFS::mount([[gnu::unused]] int what) 
{
    /* --------------- Add all of the storage drives --------------- */
    StorageDriver* storage_driver = (StorageDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);
    
    // Only up to 26 drives are supported (sda - sdz)
    int i;
    char c;
    for (i = 0, c = 'a'; i < storage_driver->get_number_of_drives() && c <= 'z'; i++, c++)
    {
        String path_str = "/sd";
        path_str += c;

        Path path(path_str);
        root_dir.append(path);
    }
}

void DevFS::umount() {} // Nothing here

fs_status_code DevFS::read(Path path, size_t count, size_t offset, char* buf) 
{

}

fs_status_code DevFS::write(Path path, size_t count, size_t offset, char* buf) 
{

}

fs_status_code DevFS::create_file([[gnu::unused]] Path path) 
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code DevFS::delete_file([[gnu::unused]] Path path) 
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code DevFS::list_files(Path path, Vector<Path>* vect) 
{
    if (!path.is_root())
        return FS_NO_SUCH_DIR; // There is only one folder in devfs - the root folder.

    *vect = root_dir;
    return FS_OK;
}
