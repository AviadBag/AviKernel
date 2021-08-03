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

    virtual void attach() = 0;
    virtual void detach() = 0;

    void set_on_tick_listener(clock_on_tick_listener);
    
    // In HZ. IF CALLED, CALL BEFORE attach(). Does not have to be called; Default value is once in every 0.01 seconds.
    void set_frequency(uint32_t);

protected:
    // A callback; Called when the clock ticks. The children has to write it as the callback.
    static void on_tick(void* context, [[gnu::unused]] uint32_t unused);

    clock_on_tick_listener listener;
    uint32_t frequency;
};

#endif // _CLOCK_DRIVER_H__