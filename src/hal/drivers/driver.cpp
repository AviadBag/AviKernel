#include "hal/drivers/driver.h"

bool Driver::get_initialized() 
{
    return initialized;
}

void Driver::initialize() 
{
    // TODO: Change it with panic
    if (get_initialized())
        return;

    initialized = true;
}
