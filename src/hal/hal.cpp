#include "hal/hal.h"
#include "hal/drivers/driver.h"

#include <cstdio.h>

void HAL::initialize() 
{
    // This is where the fun begins!
    
    // Fill general_drivers[] with nullptr's, so if a device does not exist - get_driver() will return nullptr.
    for (int i = 0; i < HAL_NUMBER_OF_GENERAL_DRIVERS; i++)
        general_drivers[i] = nullptr;
}

Driver* HAL::get_driver(HAL_GENERAL_DRIVER_TYPE driver_type) 
{
    return general_drivers[driver_type];
}

/* ------------------------- Singelton stuff. ------------------------- */
HAL* HAL::instance = nullptr;

HAL::HAL() {}

HAL* HAL::get_instance() 
{
    if (!instance)
        instance = new HAL;

    return instance;
}
