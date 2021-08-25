#ifndef __FAT32_DIRECTORY_ENTRY_H__
#define __FAT32_DIRECTORY_ENTRY_H__

#include <stdint.h>
#include <libc++/string.h>

// This class represents a single FAT32 directory entry. Can represent both a short and a long entry.
// Methods starting with 'le' are methods regarding only long entries. (is_long_entry() is true).
// Methods starting with 'se' are methods regarding only short entries. (is_long_entry() is false).

class FAT32DirectoryEntry
{
public:
    // Gets the bytes themselves.
    FAT32DirectoryEntry(uint32_t bytes);

    // ---------------------- Getters ----------------------
    bool is_used(); // If false - this does not represent a file.
    bool is_last(); // If true - it is the last entry of this directory, AND it is not used.
    bool is_directory();
    bool is_archive();
    bool is_long_entry();
    
    String se_get_name();
    bool se_is_readonly();
    bool se_is_hidden();
    bool se_is_system_file();
    bool se_is_volume_label();
    uint8_t  se_get_creation_time_tenth();
    uint16_t se_get_creation_time();
    uint16_t se_get_creation_date();
    uint16_t se_get_last_access_date();
    uint32_t se_get_first_cluster();
    uint16_t se_get_last_write_time();
    uint16_t se_get_last_write_date();
    uint32_t se_get_file_size_bytes(); // Only valid when this is not a directory

    uint8_t le_get_order(); // What long entry is it?
    bool le_is_last();      // Is it the last long entry?
    String le_get_name();

    uint32_t get_bytes();

private:
    uint32_t bytes; // The internal representation of the bytes themselves.
};

#endif // __FAT32_DIRECTORY_ENTRY_H__