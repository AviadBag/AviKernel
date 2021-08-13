#include "drivers_manager/drivers/storage/ide/storage_ide_compatibility_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"
#include "drivers_manager/drivers_manager.h"

#include "utils/vector.h"
#include "utils/io.h"

#include <stdint.h>
#include <cstdio.h>

// ICD stands for IDE Compatibility Driver

#define ICD_PCI_CLASS_MASS_STORAGE_CONTROLLER 0x1
#define ICD_PCI_SUBCLASS_IDE_CONTROLLER       0x1

// PC stands for Primary Channel, SC stands for Secondary Channel
#define ICD_PC_IN_PCI_MODE_MASK (1 << 0)     // Is the primary channel in PCI mode RIGHT NOW?
#define ICD_PC_SUPPORTS_SWITCH_MASK (1 << 1) // Can the primary channel switch modes?
#define ICD_SC_IN_PCI_MODE_MASK (1 << 2)     // Is the secondary channel in PCI mode RIGHT NOW?
#define ICD_SC_SUPPORTS_SWITCH_MASK (1 << 3) // Can the secondary channel switch modes?

// Ports bases
#define ICD_PC_COMMAND_PORTS_BASE 0x1F0
#define ICD_SC_COMMAND_PORTS_BASE 0x170

#define ICD_PC_CONTROL_PORTS_BASE 0x3F6
#define ICD_SC_CONTROL_PORTS_BASE 0x376 

// Ports
#define ICD_CONTROL_REGISTER_OFFSET       0x0
#define ICD_ALTSTATUS_REGISTER_OFFSET     0x0
#define ICD_SELECT_DRIVER_REGISTER_OFFSET 0x6

StorageIDECompatibilityDriver::StorageIDECompatibilityDriver() 
{
    pci_driver = (PCIDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
}

StorageIDECompatibilityDriver::~StorageIDECompatibilityDriver() 
{
    if (ide_controller)
        delete ide_controller;
}

uint32_t StorageIDECompatibilityDriver::get_command_port_address(icd_channel c, int offset) 
{
    uint32_t base;
    switch (c)
    {
        case ICD_PRIMARY_CHANNEL:
            base = ICD_PC_COMMAND_PORTS_BASE;
            break;
        case ICD_SECONDARY_CHANNEL:
            base = ICD_SC_COMMAND_PORTS_BASE;
            break;
        default:
            panic("StorageIDECompatibilityDriver::get_command_port_address: Invalid given channel!\n");
    }

    return base + offset;
}

uint32_t StorageIDECompatibilityDriver::get_control_port_address(icd_channel c, int offset) 
{
    uint32_t base;
    switch (c)
    {
        case ICD_PRIMARY_CHANNEL:
            base = ICD_PC_CONTROL_PORTS_BASE;
            break;
        case ICD_SECONDARY_CHANNEL:
            base = ICD_SC_CONTROL_PORTS_BASE;
            break;
        default:
            panic("StorageIDECompatibilityDriver::get_control_port_address: Invalid given channel!\n");
    }

    return base + offset;
}

void StorageIDECompatibilityDriver::write_command_port(uint8_t data, icd_channel c, int offset) 
{
    IO::outb(get_command_port_address(c, offset), data);
}

void StorageIDECompatibilityDriver::write_control_port(uint8_t data, icd_channel c, int offset) 
{
    IO::outb(get_control_port_address(c, offset), data);
}

uint8_t StorageIDECompatibilityDriver::read_command_port(icd_channel c, int offset) 
{
    return IO::inb(get_command_port_address(c, offset));
}

uint8_t StorageIDECompatibilityDriver::read_control_port(icd_channel c, int offset) 
{
    return IO::inb(get_control_port_address(c, offset));
}

void StorageIDECompatibilityDriver::disable_interrupts(icd_channel which) 
{
    write_control_port(0b0010, which, ICD_CONTROL_REGISTER_OFFSET);
}

void StorageIDECompatibilityDriver::setup_driver_and_device() 
{
    // Get the IDE controller
    get_pci_ide_controller(ide_controller);

    // Disable interrupts for both channels
    disable_interrupts(ICD_PRIMARY_CHANNEL);
    disable_interrupts(ICD_SECONDARY_CHANNEL);
}

bool StorageIDECompatibilityDriver::exist()
{
    // Check in the PCI if there is an IDE controller
    PCIDevice _ide_controller;
    if (!get_pci_ide_controller(&_ide_controller))
        return false;
    printf("Found an IDE Controller! PROG IF = 0x%X\n", pci_driver->get_prog_if(_ide_controller));
    
    // Check if at least one of the channels support compitability mode
    return channel_supports_compatibility_mode(ICD_PRIMARY_CHANNEL) && channel_supports_compatibility_mode(ICD_SECONDARY_CHANNEL);
}

bool StorageIDECompatibilityDriver::channel_supports_compatibility_mode(icd_channel channel) 
{
    uint8_t now_mask, can_switch_mask;
    switch (channel)
    {
        case ICD_PRIMARY_CHANNEL:
            now_mask = ICD_PC_IN_PCI_MODE_MASK;
            can_switch_mask = ICD_PC_SUPPORTS_SWITCH_MASK;
            break;
        case ICD_SECONDARY_CHANNEL:
            now_mask = ICD_SC_IN_PCI_MODE_MASK;
            can_switch_mask = ICD_SC_SUPPORTS_SWITCH_MASK;
            break;
        default:
            panic("StorageIDECompatibilityDriver::channel_supports_compatibility_mode: Invalid given channel!\n");
    }

    PCIDevice _ide_controller;
    get_pci_ide_controller(&_ide_controller);
    
    uint8_t prog_if = pci_driver->get_prog_if(_ide_controller);
    return (prog_if & can_switch_mask) || ((prog_if & now_mask) == 0);
}

bool StorageIDECompatibilityDriver::get_pci_ide_controller(PCIDevice* device_p) 
{
    Vector<PCIDevice>* pci_devices = pci_driver->get_devices();
    for (int i = 0; i < pci_devices->size(); i++)
    {
        PCIDevice device = pci_devices->get(i);
        uint8_t class_code = pci_driver->get_class_code(device);
        uint8_t sub_class_code = pci_driver->get_sub_class_code(device);
        if (class_code == ICD_PCI_CLASS_MASS_STORAGE_CONTROLLER && sub_class_code == ICD_PCI_SUBCLASS_IDE_CONTROLLER)
        {
            *device_p = device;
            return true;
        }
    }

    return false;
}

void StorageIDECompatibilityDriver::read_sector([[gnu::unused]] uint64_t lba) 
{

}

void StorageIDECompatibilityDriver::write_sector([[gnu::unused]] uint64_t lba, [[gnu::unused]] char* sector) 
{

}
