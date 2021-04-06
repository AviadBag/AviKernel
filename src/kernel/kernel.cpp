#include <drivers/terminal/terminal.h>
#include <drivers/pic/pic.h>
#include <drivers/pit/pit.h>

#include <kernel/gdt/gdt.h>
#include <kernel/idt/idt.h>

#include <cstdlib.h>

Terminal terminal;

void on_tick(uint32_t unused)
{
    static int counter = 0;
    counter++;
    if (counter % 100 == 0) terminal.print("Second!\n");
}

extern "C" void kernel_main(void)
{
	terminal.clear();
	terminal.print("Hello, Kernel!\n");

    GDT gdt;
    gdt.install();

    PIC::remap_irq();
    PIC::enable_all_interrupts();

	IDT idt;
	idt.install();
	asm("sti");

	PIT::initialize(100, on_tick); // Once every 0.01 second

	while (1) {}
}