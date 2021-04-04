#include <drivers/terminal/terminal.h>
#include <drivers/pic/pic.h>

#include <kernel/gdt/gdt.h>
#include <kernel/idt/idt.h>

#include <cstdlib.h>

Terminal terminal;
extern "C" void kernel_main(void)
{
	terminal.clear();
	terminal.print("Hello, Kernel!\n");

	GDT gdt;
	gdt.install();

	IDT idt;
	idt.install();

	PIC::remap_irq();
	PIC::enable_all_interrupts();
}