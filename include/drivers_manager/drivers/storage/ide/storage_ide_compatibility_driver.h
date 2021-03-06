#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/storage/ide/ide_controller.h"
#include "drivers_manager/drivers/storage/ide/physical_ide_drive.h"
#include "drivers_manager/drivers/storage/storage_driver.h"

#include <stdint.h>

class StorageIDECompatibilityDriver : public StorageDriver {
public:
    StorageIDECompatibilityDriver();
    ~StorageIDECompatibilityDriver();

    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    virtual void read_sectors(int drive, uint64_t lba, uint8_t count, char* buffer) override;
    virtual void write_sectors(int drive, uint64_t lba, uint8_t count, char* sector) override;

private:
    // Iterates the PCI to find the IDE controller; Returns false if not found.
    bool get_pci_ide_controller(PCIDevice*);

    bool channel_supports_compatibility_mode(uint8_t channel);
    bool channel_in_compatibility_mode(uint8_t channel);
    bool channel_supports_switch(uint8_t channel);
    void switch_channel_to_compatibility_mode(uint8_t channel); // Will only work if this channel support it.

    void disable_interrupts(uint8_t channel);
    void detect_drives(); // Finds all of the connected drives, adds them to the drives array, and updates the "number_of_drives" var.
    void ide_select_drive(uint8_t channel, uint8_t drive, bool lba); // Selects a drive internally.
    IDEController* get_ide_controller(uint8_t channel); // Returns the appropriate IDE controller for the given channel.
    void add_drive(uint8_t channel, uint8_t drive, uint16_t* buf); // Adds a drive to the drives list; Gets the device's buffer as an input.

    void fill_address_48_bits(IDEController* controller, uint64_t lba);
    void fill_address_28_bits(IDEController* controller, uint64_t lba);
    void fill_address_chs(IDEController* controller, uint64_t lba);

    // Does the neccessary setup before I/O: Selects drive, fills the address and writes sector count.
    void setup_rw_48_bits(PhysicalIDEDrive* drive, IDEController* controller, uint64_t lba, uint8_t count);

    PCIDriver* pci_driver;
    IDEController *primary_ide_controller, *secondary_ide_controller;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__
