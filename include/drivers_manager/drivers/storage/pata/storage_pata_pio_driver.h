#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"

class StoragePataPIODriver : public StorageDriver
{
    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    virtual void read_sector(uint64_t lba) override;
    virtual void write_sector(uint64_t lba, char* sector) override;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__