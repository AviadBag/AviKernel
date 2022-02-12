#include "fs/fat32/fat32.h"
#include "fs/vfs/vfs.h"

#include <posix/errno.h>
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

uint64_t FAT32::read(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t count, __attribute__((unused)) uint64_t offset, __attribute__((unused)) char *buf)
{
    set_errno(ENOTSUP);
    return false;
}

uint64_t FAT32::write(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t count, __attribute__((unused)) uint64_t offset, __attribute__((unused)) char *buf)
{
    set_errno(ENOTSUP);
    return false;
}

uint64_t FAT32::get_file_size(__attribute__((unused)) Path path, __attribute__((unused)) uint64_t *size)
{
    set_errno(ENOTSUP);
    return false;
}

uint64_t FAT32::create_file(__attribute__((unused)) Path path)
{
    set_errno(ENOTSUP);
    return false;
}

uint64_t FAT32::delete_file(__attribute__((unused)) Path path)
{
    set_errno(ENOTSUP);
    return false;
}

uint64_t FAT32::list_files(Path path, Vector<Path> *)
{
    set_errno(ENOTSUP);
    return false;
}

bool FAT32::file_exist(__attribute__((unused)) Path path)
{
    return false;
}

uint64_t FAT32::sector_to_offset(sector_number sector)
{
    return sector * boot_sector.bytes_per_sector;
}

cluster_number FAT32::get_next_cluster(cluster_number cluster)
{
}

bool FAT32::is_last_cluster(cluster_number cluster)
{
    cluster &= 0x0FFFFFFF;
    return cluster >= 0x0FFFFFF8 && cluster <= 0x0FFFFFFF;
}