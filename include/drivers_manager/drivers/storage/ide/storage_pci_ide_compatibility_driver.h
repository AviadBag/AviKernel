#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"

class StoragePCIIDECompatibilityDriver : public StorageDriver
{
public:
    StoragePCIIDECompatibilityDriver();
    ~StoragePCIIDECompatibilityDriver();

    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    virtual void read_sector(uint64_t lba) override;
    virtual void write_sector(uint64_t lba, char* sector) override;

private:
    PCIDriver* pci_driver;
    PCIDevice* ide_controller = nullptr;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__