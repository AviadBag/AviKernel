#include "drivers_manager/drivers/storage/storage_driver.h"

StorageDriver::~StorageDriver() 
{
    for (int i = 0; i < drives.size(); i++)
        delete drives.get(i);
}

void StorageDriver::select_drive(int d) 
{
    selected_drive = d;
}

int StorageDriver::get_number_of_drives() 
{
    return number_of_drives;
}


PhysicalDrive* StorageDriver::get_drive(int i) 
{
    return drives.get(i);
}
