#include "kernel/idt/isr.h"
#include "drivers/pic/pic.h"
#include <cstdlib.h>

#define IRQ_START 32
#define IRQ_END   47

isr_ptr isrs[NUMBER_OF_INTERRUPTS] = {};

extern "C" void isr_handler(struct registers regs)
{
    // Send EOI if this is an IRQ.
    if (regs.interrupt_number >= IRQ_START && regs.interrupt_number <= IRQ_END)
    {
        int irq = regs.interrupt_number - IRQ_START;
        PIC::send_end_of_interrupt(irq);
    }

    // Call the registered handler
    if (isrs[regs.interrupt_number] != 0) isrs[regs.interrupt_number](regs.err_code);
}

void register_isr(int interrupt_number, isr_ptr isr)
{
    isrs[interrupt_number] = isr;
}
