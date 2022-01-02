#ifndef __FAT32_DIRECTORY_ENTRY_H__
#define __FAT32_DIRECTORY_ENTRY_H__

#include <stdint.h>
#include <libc++/string.h>

// This class represents a single FAT32 directory entry. Can represent both a short and a long entry.
// Methods starting with 'le' are methods regarding only long entries. (is_long_entry() is true).
// Methods starting with 'se' are methods regarding only short entries. (is_long_entry() is false).

#define FAT32_DIR_ENTRY_LENGTH 32

class FAT32DirectoryEntry
{
public:
    // Gets the bytes themselves.
    FAT32DirectoryEntry(uint8_t* bytes); // Given an array of 32 bytes. Copies it to an inner array.

    // ---------------------- Getters ----------------------
    bool is_used(); // If true - you surely should use it.
    bool is_free(); // If true - this does not represent a file.
    bool is_end(); // If true - it is the last entry of this directory, AND it is not used.
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

    uint8_t* get_bytes(); // Returns the inner bytes array. (Not the given array in the constructor).

private:
    uint8_t get_attributes();

    uint8_t bytes[FAT32_DIR_ENTRY_LENGTH]; // The internal representation of the bytes themselves.
};

#endif // __FAT32_DIRECTORY_ENTRY_H__