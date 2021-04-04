#ifndef _PIT_H
#define _PIT_H

#include <cstdint.h>
#include <drivers/serial_ports/serial_ports.h>

class PIT
{
    public:
        void initialize();

    private:
        static void on_tick();
        static uint64_t ticks_count;
};

#endif