#ifndef _PIT_H
#define _PIT_H

#include "drivers/serial_ports/serial_ports.h"

#include <stdint.h>

#define PIT_ON_TICK_PTRS_MAX 128

typedef void (*on_tick_ptr)(uint64_t time);

class PIT {
public:
    static void initialize(uint32_t frequency);
    static void on_tick(uint32_t unsued); // Just a listener. Should not be called!
    static bool add_on_tick_listener(on_tick_ptr ptr); // Returns false if there is the listeners list is already full.

private:
    static uint64_t ticks_count;
    static on_tick_ptr on_tick_ptrs_arr[PIT_ON_TICK_PTRS_MAX];
    static int on_tick_ptrs_arr_next;
};

#endif