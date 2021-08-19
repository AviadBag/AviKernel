#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/storage/ide/ide_controller.h"
#include "drivers_manager/drivers/pci/pci_driver.h"

#include <stdint.h>

struct icd_drive
{
    uint8_t channel;
    uint8_t drive;
    bool supports_lba;
    bool address_48_bits_mode; // If false - 28 bits.
    uint64_t number_of_sectors;
};

class StorageIDECompatibilityDriver : public StorageDriver
{
public:
    StorageIDECompatibilityDriver();
    ~StorageIDECompatibilityDriver();

    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    virtual void select_drive(int d) override;

    virtual void read_sector(uint64_t lba, char* buffer) override;
    virtual void write_sector(uint64_t lba, char* sector) override;

private:
    // Iterates the PCI to find the IDE controller; Returns false if not found.
    bool get_pci_ide_controller(PCIDevice*);

    // Returns true if the channel is now in compatibility mode, or if it can be switched to compatibility mode.
    // Must be only called if THERE IS an IDE controller.
    bool channel_supports_compatibility_mode(uint8_t channel);

    void disable_interrupts(uint8_t channel);
    void detect_drives(); // Finds all of the connected drives, adds them to the drives array, and updates the "number_of_drives" var.
    void ide_select_drive(uint8_t channel, uint8_t drive, bool lba); // Selects a drive internally.
    IDEController* get_ide_controller(uint8_t channel); // Returns the appropriate IDE controller for the given channel.
    void add_drive(uint8_t channel, uint8_t drive, uint16_t* buf); // Adds a drive to the drives list; Gets the device's buffer as an input.

    void read_sector_48_bits(uint64_t lba, char* buffer);
    void read_sector_28_bits(uint64_t lba, char* buffer);
    void read_sector_chs(uint64_t lba, char* buffer);

    PCIDriver* pci_driver;
    IDEController *primary_ide_controller, *secondary_ide_controller;
    Vector<icd_drive> drives;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__