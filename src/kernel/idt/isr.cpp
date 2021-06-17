#include "kernel/idt/isr.h"
#include "drivers/terminal/terminal.h"
#include "drivers/pic/pic.h"
#include <cstdlib.h>

#define IRQ_START 32
#define IRQ_END   47

isr_ptr isrs[NUMBER_OF_INTERRUPTS] = {};

extern Terminal terminal;
extern "C" void isr_handler(struct registers regs)
{
    // Send EOI if this is an IRQ.
    if (regs.interrupt_number >= IRQ_START && regs.interrupt_number <= IRQ_END)
    {
        int irq = regs.interrupt_number - IRQ_START;
        PIC::send_end_of_interrupt(irq);
    }

//    terminal.print("Interrupt! number = ");
//
//    char number[] = {0, 0};
//    itoa(regs.interrupt_number, number, 10);
//    terminal.print(number);
//    terminal.print(", error_code = ");
//
//    char error_code[] = {0, 0};
//    itoa(regs.err_code, error_code, 10);
//    terminal.print(error_code);
//
//    terminal.print("\n");

    // Call the registered handler
    if (isrs[regs.interrupt_number] != 0) isrs[regs.interrupt_number](regs.err_code);
}

void register_isr(int interrupt_number, isr_ptr isr)
{
    isrs[interrupt_number] = isr;
}
