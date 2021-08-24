#ifndef _CLOCK_PIT_DRIVER_H__
#define _CLOCK_PIT_DRIVER_H__

#include "drivers_manager/drivers/clock/clock_driver.h"

class ClockPITDriver : public ClockDriver {
public:
    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

private:
    void configure_pit(); // Sends the required values to the PIT.
};

#endif // _CLOCK_PIT_DRIVER_H__