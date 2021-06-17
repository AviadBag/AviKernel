#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/terminal/terminal.h"

#include "kernel/gdt/gdt.h"
#include "kernel/idt/idt.h"

#include <cstdio.h>
#include <cstdlib.h>

void on_tick(uint32_t unused)
{
    static int counter = 0;
    counter++;
    if (counter % 100 == 0) // This is a second
    {
        int seconds = counter / 100;
        printf("Second: %d\n", seconds);
    }
}

void interrupt_3(uint32_t unused)
{
    printf("Interrupt 3\n", unused);
}

extern "C" void kernel_main(void)
{
    Terminal::initialize();
    printf("Hello! Welcome to AviKernel!\n");

    GDT::initialize();

    PIC::initialize();
    PIC::enable_all_interrupts();

    IDT::initialize();
    asm("sti");

    PIT::initialize(100, on_tick); // Once every 0.01 second

    while (1) { }
}