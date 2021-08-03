#include "hal/drivers/clock/clock_driver.h"

void ClockDriver::set_on_tick_listener(clock_on_tick_listener l) { listener = l; }

void ClockDriver::set_frequency(uint32_t f) { frequency = f; }
