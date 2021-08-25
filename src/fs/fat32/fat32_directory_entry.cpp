#include "fat32_directory_entry.h"

FAT32DirectoryEntry::FAT32DirectoryEntry(uint8_t* bytes) 
{   
    // Copy the given array into an inner array.
    for (int i = 0; i < FAT32_DIR_ENTRY_LENGTH; i++)
        this->bytes[i] = bytes[i];
}

uint8_t* FAT32DirectoryEntry::get_bytes() 
{
    return bytes;
}

uint8_t FAT32DirectoryEntry::get_attributes() 
{
    return bytes[11];
}

// ---------------------- Getters ----------------------

bool FAT32DirectoryEntry::is_used() 
{
    return (!is_free()) && (!is_end())
}

bool FAT32DirectoryEntry::is_free() 
{
    return bytes[0] == 0xE5;
}

bool FAT32DirectoryEntry::is_end() 
{
    return bytes[0] == 0x0;
}

bool FAT32DirectoryEntry::is_directory() 
{
    return get_attributes() & 0x10;
}

bool FAT32DirectoryEntry::is_archive() 
{
    return get_attributes() & 0x20;
}

bool FAT32DirectoryEntry::is_long_entry() 
{
    return get_attributes() & 0x0F;
}
