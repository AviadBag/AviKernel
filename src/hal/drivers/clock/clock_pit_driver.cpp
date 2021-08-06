#include "hal/drivers/clock/clock_pit_driver.h"
#include "kernel/interrupts/interrupts_manager.h"
#include "drivers/serial_ports/serial_ports.h"

#define PIT_DIVIDEND 1193180

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_0_PORT 0x40

#define PIT_IRQ_NUMBER 32

void ClockPITDriver::attach() 
{
    configure_pit();
    InterruptsManager::get_instance()->set_isr(PIT_IRQ_NUMBER, on_tick, this); // Register the callback
}

void ClockPITDriver::detach() 
{
    // TODO: de-register the callback.
}

bool ClockPITDriver::exist() 
{
    return true; // TODO: Implement it.
}

void ClockPITDriver::configure_pit() 
{
    // First, program the PIT to the desired frequency
    uint32_t divisor = PIT_DIVIDEND / frequency;

    // send the command byte
    SerialPorts::outb(PIT_COMMAND_PORT, 0x36);

    // Divisor should be sent byte by byte
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    SerialPorts::outb(PIT_CHANNEL_0_PORT, low);
    SerialPorts::outb(PIT_CHANNEL_0_PORT, high);    
}

