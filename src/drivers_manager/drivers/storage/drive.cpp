#include "drivers_manager/drivers/storage/drive.h"

PhysicalDrive::PhysicalDrive(uint32_t sector_size, uint64_t number_of_sectors) 
{
    this->sector_size = sector_size;
    this->number_of_sectors = number_of_sectors;
}

uint32_t PhysicalDrive::get_sector_size() 
{
    return sector_size;
}

uint64_t PhysicalDrive::get_number_of_sectors() 
{
    return number_of_sectors;
}

double PhysicalDrive::get_size_by() 
{
    return number_of_sectors * 512;
}

double PhysicalDrive::get_size_kb() 
{
    return get_size_by() / 1024;
}

double PhysicalDrive::get_size_mb() 
{
    return get_size_kb() / 1024;
}

double PhysicalDrive::get_size_gb() 
{
    return get_size_mb() / 1024;
}
