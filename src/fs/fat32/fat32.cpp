#include "fs/fat32/fat32.h"
#include "fs/vfs/vfs.h"

#include <cstdio.h>

void FAT32::mount(Path path)
{
    // Get the file pointer for the drive
    raw_disk = VFS::get_instance()->open(path.to_string().c_str(), O_RDWR);
    if (raw_disk == VFS_ERROR)
    {
        panic("Error while mounting FAT32");
    }

    // Get boot sector
    if (VFS::get_instance()->read(raw_disk, &boot_sector, 512) == VFS_ERROR)
    {
        panic("Error while mounting FAT32");
    }

    printf("Mounting FAT32 fs from %s, OEM name: %s", path.to_string().c_str(), boot_sector.oem_name);
}

void FAT32::umount()
{
}

fs_status_code FAT32::read(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t count, __attribute__((unused)) uint64_t offset, __attribute__((unused)) char *buf)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code FAT32::write(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t count, __attribute__((unused)) uint64_t offset, __attribute__((unused)) char *buf)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code FAT32::get_file_size(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t *size)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code FAT32::create_file(__attribute__((unused)) Path path)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code FAT32::delete_file(__attribute__((unused)) Path path)
{
    return FS_UNSUPPORTED_OPERATION;
}

fs_status_code FAT32::list_files(__attribute__((unused)) Path path, __attribute__((unused)) Vector<Path> *)
{
    return FS_UNSUPPORTED_OPERATION;
}

bool FAT32::file_exist(__attribute__((unused)) Path path)
{
    return false;
}
