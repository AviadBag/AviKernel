#include "drivers_manager/drivers/storage/pata/storage_pata_pio_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"
#include "drivers_manager/drivers_manager.h"

#include "utils/vector.h"

#include <cstdio.h>

#define PCI_CLASS_MASS_STORAGE_CONTROLLER 0x1
#define PCI_SUBCLASS_IDE_CONTROLLER       0x1

StoragePataPIODriver::StoragePataPIODriver() 
{
    pci_driver = (PCIDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
}

StoragePataPIODriver::~StoragePataPIODriver() 
{
    if (ide_controller)
        delete ide_controller;
}

void StoragePataPIODriver::setup_driver_and_device() 
{

}

bool StoragePataPIODriver::exist() 
{
    // Check in the PCI if there is an IDE controller
    Vector<PCIDevice>* pci_devices = pci_driver->get_devices();
    for (int i = 0; i < pci_devices->size(); i++)
    {
        PCIDevice device = pci_devices->get(i);
        uint8_t class_code = pci_driver->get_class_code(device);
        uint8_t sub_class_code = pci_driver->get_sub_class_code(device);
        if (class_code == PCI_CLASS_MASS_STORAGE_CONTROLLER && sub_class_code == PCI_SUBCLASS_IDE_CONTROLLER)
        {
            ide_controller = new PCIDevice(device);
            printf("Found an IDE Controller! PROG IF = 0x%X\n", pci_driver->get_prog_if(*ide_controller));
            return true;
        }
    }
    
    return false;
}

void StoragePataPIODriver::read_sector([[gnu::unused]] uint64_t lba) 
{

}

void StoragePataPIODriver::write_sector([[gnu::unused]] uint64_t lba, [[gnu::unused]] char* sector) 
{

}
