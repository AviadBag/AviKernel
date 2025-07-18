#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/driver.h"
#include "drivers_manager/drivers/keyboard/keyboard_generic_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pic/pic_driver.h"
#include "drivers_manager/drivers/storage/ide/storage_ide_compatibility_driver.h"
#include "drivers_manager/drivers/video/video_driver.h"

#include "drivers_manager/drivers/clock/clock_pit_driver.h"

#include <cstdio.h>

void DriversManager::initialize()
{
    // This is where the fun begins!

    // Fill general_drivers[] with devices!
    general_drivers[DRIVERS_MANAGER_KEYBOARD_DRIVER] = new KeyboardGenericDriver();
    general_drivers[DRIVERS_MANAGER_CLOCK_DRIVER] = new ClockPITDriver();
    general_drivers[DRIVERS_MANAGER_PCI_DRIVER] = new PCIDriver();
    general_drivers[DRIVERS_MANAGER_PIC_DRIVER] = new PICDriver();
    general_drivers[DRIVERS_MANAGER_STORAGE_DRIVER] = new StorageIDECompatibilityDriver();
    general_drivers[DRIVERS_MANAGER_VIDEO_DRIVER] = new VideoDriver();
}

Driver *DriversManager::get_driver(DRIVERS_MANAGER_GENERAL_DRIVER_TYPE driver_type)
{
    return general_drivers[driver_type];
}

/* ------------------------- Singelton stuff. ------------------------- */
DriversManager *DriversManager::instance = nullptr;

DriversManager::DriversManager() {}

DriversManager *DriversManager::get_instance()
{
    if (!instance)
        instance = new DriversManager;

    return instance;
}
