#include "hal/drivers/clock/clock_driver.h"

ClockDriver::ClockDriver() : Driver()
{
    frequency = 100; // Default value - once in every 0.01 seconds.
}

void ClockDriver::set_on_tick_listener(clock_on_tick_listener l) { listener = l; }

void ClockDriver::set_frequency(uint32_t f) { frequency = f; }

void ClockDriver::on_tick(void* context, [[gnu::unused]] uint32_t unused) 
{
    ClockDriver* thi = (ClockDriver*) context; // "thi" ==> this.
    if (thi->listener)
        thi->listener();
}
