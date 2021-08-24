#include "utils/time.h"
#include "drivers_manager/drivers/clock/clock_driver.h"
#include "drivers_manager/drivers_manager.h"

uint64_t Time::counter_ms = 0;

void Time::initialize()
{
    ClockDriver* cd = (ClockDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_CLOCK_DRIVER);
    cd->set_on_tick_listener(Time::on_tick);
}

void Time::sleep(uint64_t ms)
{
    uint64_t previous = counter_ms;
    while (previous + ms > counter_ms)
        ;
}

void Time::on_tick()
{
    counter_ms++;
}