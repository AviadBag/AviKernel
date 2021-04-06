#include <drivers/pit/pit.h>
#include <drivers/serial_ports/serial_ports.h>

#define CHANNEL_0_PORT 0x40
#define COMMAND_PORT   0x43

#define IRQ0 32 // IRQ 0

uint64_t PIT::ticks_count = 0;

void PIT::initialize(uint32_t frequency, isr_ptr on_tick)
{
    // First, program the PIT to the desired frequency
    uint32_t divisor = 1193180 / frequency;

    // send the command byte
    SerialPorts::outb(COMMAND_PORT, 0x36);

    // Divisor should be sent byte by byte
    uint8_t low  = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

    SerialPorts::outb(CHANNEL_0_PORT, low);
    SerialPorts::outb(CHANNEL_0_PORT, high);

    // Now, register my tick callback.
    register_isr(IRQ0, on_tick);
}

