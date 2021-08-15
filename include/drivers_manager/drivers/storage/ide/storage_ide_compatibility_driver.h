#ifndef __STORAGE_PATA_PIO_DRIVER_H__
#define __STORAGE_PATA_PIO_DRIVER_H__

#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"

#include <stdint.h>

struct icd_drive
{
    uint8_t channel;
    uint8_t drive;
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
    bool channel_supports_compatibility_mode(uint8_t);

    uint32_t get_command_port_address(uint8_t, int offset);
    uint32_t get_control_port_address(uint8_t, int offset);

    uint8_t read_command_port(uint8_t channel, int offset);
    uint8_t read_control_port(uint8_t channel, int offset);
    uint8_t read_status(uint8_t channel);

    void write_command_port(uint8_t data, uint8_t, int offset);
    void write_control_port(uint8_t data, uint8_t, int offset);

    void send_command(uint8_t command, uint8_t channel);

    void disable_interrupts(uint8_t which);
    void detect_drives(); // Finds all of the connected drives, adds them to the drives array, and updates the "number_of_drives" var.
    void ide_select_drive(uint8_t channel, uint8_t drive); // Selects a drive internally.

    PCIDriver* pci_driver;
    PCIDevice* ide_controller = nullptr;

    Vector<icd_drive> drives;
};

#endif // __STORAGE_PATA_PIO_DRIVER_H__