#include <kernel/idt/isr.h>
#include <drivers/terminal/terminal.h>
#include <cstdlib.h>

extern Terminal terminal;

extern "C" void isr_handler(struct registers regs)
{
    terminal.print("Interrupt: number=");

    char s1[] = {0, 0};
    itoa(regs.interrupt_number, s1, 10);
    terminal.print(s1);

    terminal.print(", error=");
    char s2[] = {0, 0};
    itoa(regs.err_code, s2, 10);
    terminal.print(s2);

    terminal.print("\n");
}