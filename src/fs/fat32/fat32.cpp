#include "fs/fat32/fat32.h"

void FAT32::mount(__attribute__((unused)) Path path)
{
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
