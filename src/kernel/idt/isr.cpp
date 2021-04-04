#include <kernel/idt/isr.h>
#include <drivers/terminal/terminal.h>
#include <cstdlib.h>

extern Terminal terminal;
extern "C" void isr_handler(struct registers regs)
{
    terminal.print("Interrupt! number = ");

    char number[] = {0, 0};
    itoa(regs.interrupt_number, number, 10);
    terminal.print(number);
    terminal.print(", error_code = ");

    char error_code[] = {0, 0};
    itoa(regs.err_code, error_code, 10);
    terminal.print(error_code);

    terminal.print("\n");
}