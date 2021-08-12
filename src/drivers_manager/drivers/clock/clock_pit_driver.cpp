#include "drivers_manager/drivers/clock/clock_pit_driver.h"
#include "kernel/interrupts/interrupts_manager.h"
#include "utils/io.h"
#include "kernel/panic.h"

#define PIT_DIVIDEND 1193180

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_0_PORT 0x40

#define PIT_IRQ_NUMBER 32

#define UINT16_T_MAX 65535 // The maximum number that can fit into uint16_t

void ClockPITDriver::setup_driver_and_device() 
{
    configure_pit();
    InterruptsManager::get_instance()->set_isr(PIT_IRQ_NUMBER, on_tick, this); // Register the callback
}

bool ClockPITDriver::exist() 
{
    return true; // TODO: Implement it.
}

void ClockPITDriver::configure_pit() 
{
    // First, program the PIT to the desired frequency
    uint32_t divisor = PIT_DIVIDEND / frequency;
    if (divisor > UINT16_T_MAX)
        panic("Cannot initial PIT with frequency of %d, because the maximum is %d (16 bits unsigned)", divisor, UINT16_T_MAX);

    // send the command byte
    IO::outb(PIT_COMMAND_PORT, 0x36);

    // Divisor should be sent byte by byte
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    IO::outb(PIT_CHANNEL_0_PORT, low);
    IO::outb(PIT_CHANNEL_0_PORT, high);    
}

