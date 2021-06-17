#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>
#include <drivers/serial_ports/serial_ports.h>
#include <kernel/idt/isr.h>

class PIT
{
    public:
        static void initialize(uint32_t frequency, isr_ptr on_tick);

    private:
        static uint64_t ticks_count;
};

#endif