#ifndef _CLOCK_DRIVER_H__
#define _CLOCK_DRIVER_H__

#include "hal/drivers/driver.h"

#include <stdint.h>

using clock_on_tick_listener = void (*)();

class ClockDriver : public Driver
{
public:
    ClockDriver();
    virtual ~ClockDriver();

    virtual void attach();
    virtual void detach();

    void set_on_tick_listener(clock_on_tick_listener);
    
    // In HZ. IF CALLED, CALL BEFORE attach(). Does not have to be called; Default value is once in every 0.01 seconds.
    void set_frequency(uint32_t);

protected:
    clock_on_tick_listener listener;
    uint32_t frequency;
};

#endif // _CLOCK_DRIVER_H__