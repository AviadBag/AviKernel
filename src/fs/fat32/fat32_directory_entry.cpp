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

// ---------------------- Shared Methods ----------------------
bool FAT32DirectoryEntry::is_used() 
{
    return (!is_free()) && (!is_end());
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

// ---------------------- Short Name Methods ----------------------
String FAT32DirectoryEntry::se_get_name() 
{
    return String((char*) bytes, 11);
}

bool FAT32DirectoryEntry::se_is_readonly() 
{
    return get_attributes() & 0x01;
}

bool FAT32DirectoryEntry::se_is_hidden() 
{
    return get_attributes() & 0x02;
}

bool FAT32DirectoryEntry::se_is_system_file() 
{
    return get_attributes() & 0x04;
}

bool FAT32DirectoryEntry::se_is_volume_label() 
{
    return get_attributes() & 0x08;
}

uint8_t FAT32DirectoryEntry::se_get_creation_time_tenth() 
{
    return bytes[13];
}

uint16_t FAT32DirectoryEntry::se_get_creation_time() 
{
    return *((uint16_t*)(&bytes[14]));
}

uint16_t FAT32DirectoryEntry::se_get_creation_date() 
{
    return *((uint16_t*)(&bytes[16]));
}

uint16_t FAT32DirectoryEntry::se_get_last_access_date() 
{
    return *((uint16_t*)(&bytes[18]));
}

uint32_t FAT32DirectoryEntry::se_get_first_cluster() 
{
    uint16_t low  = *((uint16_t*)(&bytes[26]));
    uint16_t high = *((uint16_t*)(&bytes[20]));
    return (high << 16) + low;
}

uint16_t FAT32DirectoryEntry::se_get_last_write_time() 
{
    return *((uint16_t*)(&bytes[22]));
}

uint16_t FAT32DirectoryEntry::se_get_last_write_date() 
{
    return *((uint16_t*)(&bytes[24]));
}

uint32_t FAT32DirectoryEntry::se_get_file_size_bytes() 
{
    return *((uint32_t*)(&bytes[28]));
}

// ---------------------- Long Name Methods ----------------------
