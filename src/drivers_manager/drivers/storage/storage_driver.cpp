#include "drivers_manager/drivers/storage/storage_driver.h"

void StorageDriver::select_drive(int d) 
{
    selected_drive = d;
}

int StorageDriver::get_number_of_drives() 
{
    return number_of_drives;
}
