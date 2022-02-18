#include "fs/fat32/fat32.h"
#include "fs/vfs/vfs.h"
#include "fs/fat32/fat32_directory_entry.h"

#include <posix/errno.h>
#include <cstdio.h>

#define FAT32_DIR_ENTRY_SIZE 32

void DumpHex(const void *data, size_t size);

void print_entry(FAT32DirectoryEntry entry)
{
    if (entry.is_long_entry())
    {
        printf("%s - long entry\n", entry.le_get_name().c_str());
    }
    else
    {
        printf("%s - short entry\n", entry.se_get_name().c_str());
    }
}

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

    // Just for testing - view the root directory
    Vector<FAT32DirectoryEntry> root_dir_entries;
    if (!read_dir(boot_sector.root_cluster, &root_dir_entries))
        panic("Error while mounting FAT32");
    for (int i = 0; i < root_dir_entries.size(); i++)
    {
        FAT32DirectoryEntry entry = root_dir_entries.get(i);
        if (entry.is_short_entry())
        {
            printf("Reading file %s...\n", entry.se_get_name().c_str());
            char *buf;
            if (!read_file(entry.se_get_file_size_bytes(), entry.se_get_first_cluster(), &buf))
                panic("Error while mounting FAT32");
            for (uint32_t i = 0; i < entry.se_get_file_size_bytes(); i++)
                putchar(buf[i]);
        }
    }

    printf("Mounting FAT32 fs from %s, OEM name: %s", path.to_string().c_str(), boot_sector.oem_name);
}

uint64_t FAT32::get_cluster_size_bytes()
{
    return boot_sector.sectors_per_cluster * boot_sector.bytes_per_sector;
}

bool FAT32::read_file(uint32_t file_size_bytes, cluster_number first_cluster, char **buf)
{
    if (file_size_bytes == 0)
        return true; // There is nothing to do..

    // Get file size and allocate buffer for it.
    *buf = new char[file_size_bytes];
    char *current_cluster_data = *buf;
    if (!current_cluster_data)
    {
        set_errno(ENOMEM);
        return false;
    }

    // Follow clusters chain
    cluster_number cluster = first_cluster;
    uint64_t number_of_clusters = file_size_bytes / get_cluster_size_bytes() + 1;
    for (uint64_t i = 0; i < number_of_clusters; i++)
    {
        if (is_last_cluster(cluster)) // Should not happen
            panic("FAT32:read_file - riched last cluster before riched end of file");

        if (i + 1 == number_of_clusters) // This is the last cluster - we should not read the whole cluster.
        {
            file_size_bytes -= get_cluster_size_bytes() * i;
            if (!VFS::get_instance()->pread(raw_disk, current_cluster_data, file_size_bytes, cluster_to_offset(cluster)))
                return false;
        }
        else
        {
            if (!VFS::get_instance()->pread(raw_disk, current_cluster_data, get_cluster_size_bytes(), cluster_to_offset(cluster)))
                return false;
        }
        current_cluster_data += get_cluster_size_bytes();
        cluster = get_next_cluster(cluster);
    }

    return true;
}

bool FAT32::read_dir(cluster_number first_cluster, Vector<FAT32DirectoryEntry> *entries)
{
    // Follow clusters chain
    cluster_number cluster = first_cluster;
    while (!is_last_cluster(cluster))
    {
        // Allocate a cluster size buffer
        uint8_t *buf = new uint8_t[get_cluster_size_bytes()];
        if (!buf)
        {
            set_errno(ENOMEM);
            return false;
        }
        if (!VFS::get_instance()->pread(raw_disk, buf, get_cluster_size_bytes(), cluster_to_offset(cluster)))
            return false;

        int number_of_entries = get_cluster_size_bytes() / FAT32_DIR_ENTRY_SIZE;
        for (int i = 0; i < number_of_entries; i++)
        {
            FAT32DirectoryEntry entry(buf + i * FAT32_DIR_ENTRY_SIZE);
            if (entry.le_is_last())
                return true; // Finish!
            entries->append(entry);
        }

        cluster = get_next_cluster(cluster);
    }

    return true;
}

void FAT32::umount()
{
    if (fat)
        delete fat;
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

uint64_t FAT32::cluster_to_offset(cluster_number cluster)
{
    uint64_t data_region_first_sector = boot_sector.reserved_sectors_count + boot_sector.number_of_fats * boot_sector.sectors_per_fat;
    uint64_t cluster_sector = ((cluster - 2) * boot_sector.sectors_per_cluster) + data_region_first_sector; // The first cluster is cluster 2
    return sector_to_offset(cluster_sector);
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