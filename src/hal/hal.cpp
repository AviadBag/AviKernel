#include "hal/hal.h"
#include "hal/drivers/driver.h"
#include "hal/drivers/keyboard/keyboard_generic_driver.h"

#include <cstdio.h>

void HAL::initialize()
{
    // This is where the fun begins!

    // Fill general_drivers[] with devices!
    general_drivers[HAL_KEYBOARD_DRIVER] = new KeyboardGenericDriver();
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