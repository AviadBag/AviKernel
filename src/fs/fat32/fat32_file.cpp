#include "fs/fat32/fat32_file.h"

#include <utils/vector.h>

FAT32File::FAT32File()
{
    _is_directory = false;
    size_bytes = 0;
    first_cluster = 0;
    name = "<no name>";
}

FAT32File::FAT32File(FAT32DirectoryEntry short_entry)
{
    Vector<FAT32DirectoryEntry> long_entries; // empty
    setup_data(short_entry, long_entries);
}

FAT32File::FAT32File(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries)
{
    setup_data(short_entry, long_entries);
}

void FAT32File::setup_data(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries)
{
    _is_directory = short_entry.is_directory();
    size_bytes = short_entry.se_get_file_size_bytes();
    first_cluster = short_entry.se_get_first_cluster();
    setup_name(short_entry, long_entries);
}

void FAT32File::setup_name(FAT32DirectoryEntry short_entry, Vector<FAT32DirectoryEntry> long_entries)
{
    if (long_entries.size() == 0) // No long entries
        setup_name_8_3(short_entry.se_get_name());
    else
        setup_name_long(long_entries);
}

void FAT32File::setup_name_8_3(String name83)
{
    // Loop until we reach a space, or until we finish the 8 part.
    for (int i = 0; i < 8; i++)
    {
        if (name83[i] == ' ') // This is the end of the 8 part.
            break;
        else
            name += name83[i];
    }

    String extension;
    // Until we finish the 3 part.
    for (int i = 8; i < 11; i++)
    {
        if (name83[i] == ' ') // Spaces are for left padding, we should ignore them.
            continue;
        else
            extension += name83[i];
    }

    if (!extension.empty())
        name += '.' + extension;
}

// A helper function, that setup_name_long is giving to vector.sort in order to sort long entries by order.
// (Returns true if first is bigger, as described in vector's docs)
bool sort_long_entries(FAT32DirectoryEntry first, FAT32DirectoryEntry second)
{
    return first.le_get_order() > second.le_get_order();
}

void FAT32File::setup_name_long(Vector<FAT32DirectoryEntry> long_entries)
{
    long_entries.sort(sort_long_entries); // Sort by order
    for (int i = 0; i < long_entries.size(); i++)
        name += long_entries.get(i).le_get_name();
}

bool FAT32File::is_directory()
{
    return _is_directory;
}

uint32_t FAT32File::get_size_bytes()
{
    return size_bytes;
}

String FAT32File::get_name()
{
    return name;
}

cluster_number FAT32File::get_first_cluster()
{
    return first_cluster;
}
