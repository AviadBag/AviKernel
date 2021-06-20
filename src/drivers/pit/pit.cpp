#include "drivers/pit/pit.h"
#include "drivers/serial_ports/serial_ports.h"

#define CHANNEL_0_PORT 0x40
#define COMMAND_PORT 0x43

#define IRQ0 32 // IRQ 0

uint64_t PIT::ticks_count;
on_tick_ptr PIT::on_tick_ptrs_arr[];
int PIT::on_tick_ptrs_arr_next;

void PIT::on_tick(uint32_t unsued)
{
    ticks_count++;
    for (int i = 0; i < on_tick_ptrs_arr_next; i++)
        on_tick_ptrs_arr[i](ticks_count);
}

bool PIT::add_on_tick_listener(on_tick_ptr ptr)
{
    if (on_tick_ptrs_arr_next == PIT_ON_TICK_PTRS_MAX)
        return false;

    on_tick_ptrs_arr[on_tick_ptrs_arr_next++] = ptr;
    return true;
}

void PIT::initialize(uint32_t frequency)
{
    ticks_count = 0;
    on_tick_ptrs_arr_next = 0;

    // First, program the PIT to the desired frequency
    uint32_t divisor = 1193180 / frequency;

    // send the command byte
    SerialPorts::outb(COMMAND_PORT, 0x36);

    // Divisor should be sent byte by byte
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    SerialPorts::outb(CHANNEL_0_PORT, low);
    SerialPorts::outb(CHANNEL_0_PORT, high);

    // Now, register my tick callback.
    register_isr(IRQ0, on_tick);
}
