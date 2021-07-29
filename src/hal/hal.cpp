#include "hal/hal.h"
#include "hal/drivers/driver.h"

#include <cstdio.h>

HAL* HAL::instance = nullptr;

HAL::HAL() {}

HAL* HAL::get_instance() 
{
    if (instance == nullptr)
    {
        kprintf("new!\n");
        instance = new HAL();
    }

    return instance;
}

void HAL::initialize() 
{
    // This is where the fun begins!
}
