#include "hal/hal.h"
#include "hal/drivers/driver.h"
#include "hal/drivers/keyboard/keyboard_generic_driver.h"
#include "hal/drivers/bus/bus_pci_driver.h"

#include "hal/drivers/clock/clock_pit_driver.h"

#include <cstdio.h>

void HAL::initialize()
{
    // This is where the fun begins!

    // Fill general_drivers[] with devices!
    general_drivers[HAL_KEYBOARD_DRIVER] = new KeyboardGenericDriver();
    
    // I don't plan on adding another clock and bus drivers, so there are no generic drivers for them.
    general_drivers[HAL_CLOCK_DRIVER] = new ClockPITDriver();
    general_drivers[HAL_BUS_DRIVER] = new BusPCIDriver();
}

Driver* HAL::get_driver(HAL_GENERAL_DRIVER_TYPE driver_type)
{
    return general_drivers[driver_type];
}

/* ------------------------- Singelton stuff. ------------------------- */
HAL* HAL::instance = nullptr;

HAL::HAL() { }

HAL* HAL::get_instance()
{
    if (!instance)
        instance = new HAL;

    return instance;
}
