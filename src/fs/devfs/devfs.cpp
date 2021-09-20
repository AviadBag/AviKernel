#include "fs/devfs/devfs.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/storage/physical_drive.h"

#include <cstdio.h>
#include <cstring.h>

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
    return io(DEVFS_OPR_READ, path, count, offset, buf);
}

fs_status_code DevFS::write(Path path, size_t count, size_t offset, char* buf) 
{
    return io(DEVFS_OPR_WRITE, path, count, offset, buf);
}

fs_status_code DevFS::io(devfs_operation operation, Path path, size_t count, size_t offset, char* buf) 
{
    // Does this file exist?
    if (!root_dir.exist(path))
        return FS_NO_SUCH_FILE;

    // Is it a storage drive?
    if (path.to_string().substr(1, 2) == "sd" && path.to_string().size() == 4)
    {
        if (operation == DEVFS_OPR_READ) return storage_drive_read(path, count, offset, buf);
        else return FS_UNSUPPORTED_OPERATION;
    }

    return FS_OK;
}

fs_status_code DevFS::storage_drive_read(Path path, size_t count, size_t offset, char* buf) 
{
    char drive_char = path.to_string()[3]; // "/sda" => 'a', "/sdz" => 'z'...
    char drive_number = drive_char - 'a';  // 'a' => 0, 'z' => 25...

    /* <count> is prhobably not sector aligned, so we have to write everything
       to a temporarily buffer with sector aligned size, and then copy just how much we need to the real buffer */

    // Gain the storage driver
    StorageDriver* storage_driver = (StorageDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);

    // Determine sector size
    uint32_t sector_size = storage_driver->get_drive(drive_number)->get_sector_size();

    // What's the LBA?
    uint64_t lba = offset / sector_size;

    // How many sectors do we have to read? (Round up)
    uint64_t starting_sector = lba;
    uint64_t ending_sector = (offset + count) / sector_size;
    uint64_t sectors_count = ending_sector - starting_sector + 1;

    // Allocate the temporarily buffer
    char* temp_buf = new char[sectors_count * sector_size];
    if (!temp_buf)
        return FS_NOT_ENOUGH_MEMORY;

    // READ!

    // We can only read up to 255 sectors at a time
    if (sectors_count <= 255)
    {
        // Well, just read...
        storage_driver->read_sectors(drive_number, lba, sectors_count, temp_buf);
    }
    else
    {
        delete temp_buf;
        return FS_UNSUPPORTED_OPERATION;
    }
    
    // Copy to the real buffer
    memcpy(buf, temp_buf + (offset % sector_size), count);
    delete temp_buf;

    return FS_OK;
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
