#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"

enum icd_channel
{
    ICD_PRIMARY_CHANNEL,
    ICD_SECONDARY_CHANNEL
};

class StorageIDECompatibilityDriver : public StorageDriver
{
public:
    StorageIDECompatibilityDriver();
    ~StorageIDECompatibilityDriver();

    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    virtual void read_sector(uint64_t lba) override;
    virtual void write_sector(uint64_t lba, char* sector) override;

private:
    // Iterates the PCI to find the IDE controller; Returns false if not found.
    bool get_pci_ide_controller(PCIDevice*);

    // Returns true if the channel is now in compatibility mode, or if it can be switched to compatibility mode.
    // Must be only called if THERE IS an IDE controller.
    bool channel_supports_compatibility_mode(icd_channel);

    PCIDriver* pci_driver;
    PCIDevice* ide_controller = nullptr;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__