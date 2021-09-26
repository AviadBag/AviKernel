#include "fs/devfs/devfs.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/storage/physical_drive.h"

#include <cstdio.h>
#include <cstring.h>

void DevFS::mount([[gnu::unused]] Path what)
{
    /* --------------- Add all of the storage drives --------------- */
    StorageDriver *storage_driver = (StorageDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);

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

fs_status_code DevFS::read(Path path, uint64_t count, uint64_t offset, char *buf)
{
    return io(DEVFS_OPR_READ, path, count, offset, buf);
}

fs_status_code DevFS::write(Path path, uint64_t count, uint64_t offset, char *buf)
{
    return io(DEVFS_OPR_WRITE, path, count, offset, buf);
}

fs_status_code DevFS::io(devfs_operation operation, Path path, uint64_t count, uint64_t offset, char *buf)
{
    // Is it a folder?
    if (path.is_folder())
        return FS_FILE_IS_REQUIRED;

    // Does this file exist?
    if (!file_exist(path))
        return FS_NO_SUCH_FILE;

    // Is it a storage drive?
    if (path.to_string().substr(1, 2) == "sd" && path.to_string().size() == 4)
    {
        if (operation == DEVFS_OPR_READ)
            return storage_drive_read(path, count, offset, buf);
        else if (operation == DEVFS_OPR_WRITE)
            return storage_drive_write(path, count, offset, buf);
        else
            panic("DevFS:io() - got an invalid operation!");
    }

    return FS_OK;
}

fs_status_code DevFS::get_file_size(Path path, uint64_t *size)
{
    // Is it a folder?
    if (path.is_folder())
        return FS_FILE_IS_REQUIRED;

    // Does this file exist?
    if (!file_exist(path))
        return FS_NO_SUCH_FILE;

    // Is it a storage drive?
    if (path.to_string().substr(1, 2) == "sd" && path.to_string().size() == 4)
    {
        char drive_char = path.to_string()[3]; // "/sda" => 'a', "/sdz" => 'z'...
        char drive_number = drive_char - 'a';  // 'a' => 0, 'z' => 25...

        // Gain the storage driver
        StorageDriver *storage_driver = (StorageDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);

        // Get the required drive
        PhysicalDrive *drive = storage_driver->get_drive(drive_number);

        *size = drive->get_size_by();
    }

    return FS_OK;
}

fs_status_code DevFS::storage_drive_read(Path path, uint64_t count, uint64_t offset, char *buf)
{
    char drive_char = path.to_string()[3]; // "/sda" => 'a', "/sdz" => 'z'...
    char drive_number = drive_char - 'a';  // 'a' => 0, 'z' => 25...

    /* <count> is prhobably not sector aligned, so we have to write everything
       to a temporarily buffer with sector aligned size, and then copy just how much we need to the real buffer */

    // Gain the storage driver
    StorageDriver *storage_driver = (StorageDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);

    // Determine sector size
    uint32_t sector_size = storage_driver->get_drive(drive_number)->get_sector_size();

    // What's the LBA?
    uint64_t lba = offset / sector_size;

    // How many sectors do we have to read? (Round up)
    uint64_t starting_sector = lba;
    uint64_t ending_sector = (offset + count) / sector_size;
    uint64_t sectors_count = ending_sector - starting_sector + 1;

    // Allocate the temporarily buffer
    char *temp_buf = new char[sectors_count * sector_size];
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
        // Well, we will be reading in groups of 255 sectors

        char *tmp = temp_buf;

        // Full jumps, of 255
        for (uint64_t i = 0; i < sectors_count / 255; i++)
        {
            storage_driver->read_sectors(drive_number, lba, 255, tmp);
            tmp += (255 * sector_size); // Advance the buffer pointer to the next 255 sectors area.
            lba += 255;
        }

        // The last read (If needed) is not 255 sectors.
        storage_driver->read_sectors(drive_number, lba, sectors_count % 255, tmp);
    }

    // Copy to the real buffer
    memcpy(buf, temp_buf + (offset % sector_size), count);
    delete[] temp_buf;

    return FS_OK;
}

fs_status_code DevFS::storage_drive_write(Path path, uint64_t count, uint64_t offset, char *buf)
{
    char drive_char = path.to_string()[3]; // "/sda" => 'a', "/sdz" => 'z'...
    char drive_number = drive_char - 'a';  // 'a' => 0, 'z' => 25...

    // Gain the storage driver
    StorageDriver *storage_driver = (StorageDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);

    // Determine sector size
    uint32_t sector_size = storage_driver->get_drive(drive_number)->get_sector_size();

    // What's the LBA?
    uint64_t lba = offset / sector_size;

    // How many sectors do we have to write? (Round up)
    uint64_t starting_sector = lba;
    uint64_t ending_sector = (offset + count) / sector_size;
    uint64_t sectors_count = ending_sector - starting_sector + 1;

    // WRITE!

    // The simplest case - We only have to write one or two sector
    if (sectors_count == 1 || sectors_count == 2)
    {
        char *sectors = new char[sector_size * sectors_count];
        if (!sectors)
            return FS_NOT_ENOUGH_MEMORY;

        // First, read the sector
        storage_driver->read_sectors(drive_number, lba, sectors_count, sectors);

        // Then, replace the new data
        uint64_t offset_in_sector = offset % sector_size;
        memcpy(sectors + offset_in_sector, buf, count);

        // And - write the updated sector
        storage_driver->write_sectors(drive_number, lba, sectors_count, sectors);

        delete[] sectors;
    }
    // The more complicated case. We have to read and update the first and last sector, and only
    // to write the sectors between.
    else
    {
        char *first_sector = new char[sector_size];
        char *last_sector = new char[sector_size];

        if (!first_sector || !last_sector)
            return FS_NOT_ENOUGH_MEMORY;

        // We will first treat the first and last sectors
        // Read them
        storage_driver->read_sectors(drive_number, starting_sector, 1, first_sector);
        storage_driver->read_sectors(drive_number, ending_sector, 1, last_sector);

        // Update them
        // First Sector
        uint64_t offset_in_first_sector = offset % sector_size;
        uint64_t count_write_in_first_sector = sector_size - offset_in_first_sector;
        memcpy(first_sector + offset_in_first_sector, buf, count_write_in_first_sector);
        // Last Sector
        uint64_t offset_in_buf = (sectors_count - 2) * sector_size + count_write_in_first_sector;
        memcpy(last_sector, buf + offset_in_buf, count - offset_in_buf);

        // Write them back
        storage_driver->write_sectors(drive_number, starting_sector, 1, first_sector);
        storage_driver->write_sectors(drive_number, ending_sector, 1, last_sector);

        // And now - update the sectors between
        uint64_t sectors_between_count = sectors_count - 2;
        lba++; // Go to the second sector

        // We can only write up to 255 sectors at a time
        if (sectors_between_count <= 255)
        {
            // Well, just write...
            storage_driver->write_sectors(drive_number, lba, sectors_between_count, buf + count_write_in_first_sector);
        }
        else
        {
            // Well, we will be writing in groups of 255 sectors

            char *buf_ptr = buf + count_write_in_first_sector;

            // Full jumps, of 255
            for (uint64_t i = 0; i < sectors_between_count / 255; i++)
            {
                storage_driver->write_sectors(drive_number, lba, 255, buf + count_write_in_first_sector);
                buf_ptr += (255 * sector_size); // Advance the buffer pointer to the next 255 sectors area.
                lba += 255;
            }

            // The last write (If needed) is not 255 sectors.
            storage_driver->read_sectors(drive_number, lba, sectors_between_count % 255, buf_ptr);
        }

        delete[] first_sector;
        delete[] last_sector;
    }

    return FS_OK;
}

bool DevFS::file_exist(Path path)
{
    if (path.is_folder())
        return path.is_root(); // We have only one folder - the root folder.

    return root_dir.exist(path);
}

fs_status_code DevFS::create_file([[gnu::unused]] Path path)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code DevFS::delete_file([[gnu::unused]] Path path)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code DevFS::list_files(Path path, Vector<Path> *vect)
{
    if (!path.is_root())
        return FS_NO_SUCH_DIR; // There is only one folder in devfs - the root folder.

    *vect = root_dir;
    return FS_OK;
}
