#include <drivers/terminal/terminal.h>
#include <drivers/pic/pic.h>

#include <kernel/gdt/gdt.h>
#include <kernel/idt/idt.h>

#include <cstdlib.h>

extern "C" void kernel_main(void) 
{
	Terminal terminal;
	terminal.clear();
	terminal.print("Hello, Kernel!\n");

	GDT gdt;
	gdt.install();

	IDT idt;
	idt.install();

	PIC pic;
	pic.remap_irq();
	pic.enable_all_interrupts();

	while (1)
		;
}