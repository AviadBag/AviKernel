#ifndef _CLOCK_PIT_DRIVER_H__
#define _CLOCK_PIT_DRIVER_H__

#include "hal/drivers/clock/clock_driver.h"

class ClockPITDriver : public ClockDriver
{
public:
    virtual void attach();
    virtual void detach();
    virtual bool exist();

private:
    void configure_pit(); // Sends the required values to the PIT.
};

#endif // _CLOCK_PIT_DRIVER_H__