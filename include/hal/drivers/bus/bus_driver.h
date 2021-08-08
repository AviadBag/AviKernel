#ifndef _BUS_DRIVER_H__
#define _BUS_DRIVER_H__

#include "hal/drivers/driver.h"
#include "hal/drivers/bus/device.h"
#include "utils/vector.h"

/* This class represents a bus driver. */

class BusDriver : public Driver
{
public:    
    virtual void attach() override;
    
    Vector<Device>* get_devices();

protected:
    virtual void enumerate_devices() = 0;
    
    Vector<Device> devices;
};

#endif // _BUS_DRIVER_H__