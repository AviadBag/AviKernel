#ifndef _STORAGE_DRIVER_H__
#define _STORAGE_DRIVER_H__

#include "drivers_manager/drivers/driver.h"
#include "drivers_manager/drivers/storage/physical_drive.h"
#include "utils/vector.h"

#include <stdint.h>

class StorageDriver : public Driver {
public:
    virtual ~StorageDriver();

    virtual void setup_driver_and_device() = 0;
    virtual bool exist() = 0;

    virtual void read_sectors(uint64_t lba, uint8_t count, char* buffer) = 0;
    virtual void write_sectors(uint64_t lba, uint8_t count, char* sector) = 0;

    virtual void select_drive(int d); // Starting from drive 0.
    int get_number_of_drives();

    PhysicalDrive* get_drive(int i);

protected:
    int selected_drive = 0;
    int number_of_drives = 0;

    Vector<PhysicalDrive*> drives;
};

#endif // _STORAGE_DRIVER_H__
