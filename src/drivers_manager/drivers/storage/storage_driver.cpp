#include "drivers_manager/drivers/storage/storage_driver.h"

void StorageDriver::select_disk(int d) 
{
    selected_disk = d;
}

int StorageDriver::get_number_of_disks() 
{
    return number_of_disks;
}
