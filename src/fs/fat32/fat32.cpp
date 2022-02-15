#include "fs/fat32/fat32.h"
#include "fs/vfs/vfs.h"

#include <posix/errno.h>
#include <cstdio.h>

void FAT32::mount(Path path)
{
    // Here I panic on error. I would like in the future to change it, to return errno. But for that I
    // will have to change fs. So later.

    // Get the file pointer for the drive
    raw_disk = VFS::get_instance()->open(path.to_string().c_str(), O_RDWR);
    if (raw_disk == VFS_ERROR)
    {
        panic("Error while mounting FAT32");
    }

    // Get boot sector
    if (!VFS::get_instance()->read(raw_disk, &boot_sector, 512))
    {
        panic("Error while mounting FAT32");
    }

    // Get the FAT.
    // First, allocate.
    fat_size_bytes = sector_to_offset(boot_sector.sectors_per_fat);
    fat = new uint32_t[fat_size_bytes];
    if (!fat)
    {
        panic("Error while mounting FAT32");
    }

    // Read!
    uint64_t fat_offset_bytes = sector_to_offset(boot_sector.reserved_sectors_count);
    printf("Reading fat... Fat's offset is %llu, Fat's size is %llu\n", fat_offset_bytes, fat_size_bytes);
    if (!VFS::get_instance()->pread(raw_disk, fat, fat_size_bytes, fat_offset_bytes))
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
    cluster &= 0x0FFFFFFF; // The first four bits should be ignored.
    return fat[cluster];
}

bool FAT32::is_last_cluster(cluster_number cluster)
{
    return cluster >= 0x0FFFFFF8 && cluster <= 0x0FFFFFFF;
}