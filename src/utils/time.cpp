#include "utils/time.h"
#include "drivers/pit/pit.h"

#include <cstdio.h>

uint64_t Time::ticks;

void Time::initialize()
{
    kprintf("Initializing Time Util...\n");
    ticks = 0;
    PIT::add_on_tick_listener(on_tick);
}

void Time::on_tick([[gnu::unused]] uint64_t unused)
{
    ticks++;
}

void Time::sleep(uint64_t ms)
{
    uint64_t current = ticks;
    while (current + ms > ticks)
        ;
}