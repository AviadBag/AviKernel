#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"

#include <stdint.h>

enum icd_channel
{
    ICD_PRIMARY_CHANNEL   = 0,
    ICD_SECONDARY_CHANNEL = 1
};

/**
 * It might be confusing, so I will explain:
 * icd_channel_drive help to identify if a drive is slave or master.
 * icd_drive represents a complete drive.
 */

enum icd_channel_drive
{
    ICD_MASTER = 0,
    ICD_SLAVE  = 1
};

struct icd_drive
{
    icd_channel channel;
    icd_channel_drive drive;
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

    uint32_t get_command_port_address(icd_channel, int offset);
    uint32_t get_control_port_address(icd_channel, int offset);

    uint8_t read_command_port(icd_channel, int offset);
    uint8_t read_control_port(icd_channel, int offset);

    void write_command_port(uint8_t data, icd_channel, int offset);
    void write_control_port(uint8_t data, icd_channel, int offset);

    void disable_interrupts(icd_channel which);

    PCIDriver* pci_driver;
    PCIDevice* ide_controller = nullptr;

    Vector<icd_drive> drives;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__