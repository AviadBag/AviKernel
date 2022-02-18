#ifndef __FAT32_FILE_H__
#define __FAT32_FILE_H__

#include "fat32_directory_entry.h"

#include <utils/vector.h>

/* Represents a fat32 file. This is a higher level representation of a directory entry. */

using cluster_number = uint64_t;

class FAT32File
{
public:
    // A long entry is not necessary.
    FAT32File(FAT32DirectoryEntry short_entry);
    FAT32File(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries);

    bool is_directory();
    uint32_t get_size_bytes();
    String get_name();
    cluster_number get_first_cluster();

private:
    // The main method - converts the short and long entries into the fields of this class.
    // <long_entries> can be empty
    void setup_data(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries);

    // Puts in <name> var the name of the file, extracted from the entiries. (long_entry is optional, like in setup_data)
    void setup_name(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries);

    // Puts in <name> the given 8.3 filename, converted to regular string.
    void setup_name_8_3(String name83);

    // Puts in <name> the long file name extracted from the given long entries.
    void setup_name_long(Vector<FAT32DirectoryEntry> long_entries);

    bool _is_directory;
    uint32_t size_bytes;
    String name;
    cluster_number first_cluster;
};

#endif // __FAT32_FILE_H__