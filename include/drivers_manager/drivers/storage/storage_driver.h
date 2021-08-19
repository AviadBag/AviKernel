#ifndef _STORAGE_DRIVER_H__
#define _STORAGE_DRIVER_H__

#include "drivers_manager/drivers/driver.h"

#include <stdint.h>

class StorageDriver : public Driver
{
public:
    virtual void setup_driver_and_device() = 0;
    virtual bool exist() = 0;

    virtual void read_sector(uint64_t lba) = 0;
    virtual void write_sector(uint64_t lba, char* sector) = 0;

    virtual void select_drive(int d); // Starting from drive 0.
    int get_number_of_drives();

protected:
    int selected_drive = 0;
    int number_of_drives = 0;
};

#endif // _STORAGE_DRIVER_H__