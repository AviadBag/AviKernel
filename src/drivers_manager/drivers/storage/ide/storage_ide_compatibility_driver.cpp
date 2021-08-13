#include "drivers_manager/drivers/storage/ide/storage_ide_compatibility_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"
#include "drivers_manager/drivers_manager.h"

#include "utils/vector.h"

#include <cstdio.h>

#define PCI_CLASS_MASS_STORAGE_CONTROLLER 0x1
#define PCI_SUBCLASS_IDE_CONTROLLER       0x1

StorageIDECompatibilityDriver::StorageIDECompatibilityDriver() 
{
    pci_driver = (PCIDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
}

StorageIDECompatibilityDriver::~StorageIDECompatibilityDriver() 
{
    if (ide_controller)
        delete ide_controller;
}

void StorageIDECompatibilityDriver::setup_driver_and_device() 
{
    // Here initialize ide_controller var.
}

bool StorageIDECompatibilityDriver::exist()
{
    // Check in the PCI if there is an IDE controller
    PCIDevice _ide_controller;
    if (!get_pci_ide_controller(&_ide_controller))
        return false;
    printf("Found an IDE Controller! PROG IF = 0x%X\n", pci_driver->get_prog_if(_ide_controller));
    
    // Check if this device suports compitability mode

    return true;
}

bool StorageIDECompatibilityDriver::get_pci_ide_controller(PCIDevice* device_p) 
{
    Vector<PCIDevice>* pci_devices = pci_driver->get_devices();
    for (int i = 0; i < pci_devices->size(); i++)
    {
        PCIDevice device = pci_devices->get(i);
        uint8_t class_code = pci_driver->get_class_code(device);
        uint8_t sub_class_code = pci_driver->get_sub_class_code(device);
        if (class_code == PCI_CLASS_MASS_STORAGE_CONTROLLER && sub_class_code == PCI_SUBCLASS_IDE_CONTROLLER)
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
