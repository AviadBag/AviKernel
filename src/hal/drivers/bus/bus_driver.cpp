#include "hal/drivers/bus/bus_driver.h"

void BusDriver::attach() 
{
    enumerate_devices();
}

Vector<Device>* BusDriver::get_devices() 
{
    return &devices;
}
