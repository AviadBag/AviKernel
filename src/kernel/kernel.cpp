#include <drivers/terminal/terminal.h>
#include <kernel/gdt/gdt.h>

extern "C" void kernel_main(void) 
{
	GDT gdt;
	gdt.init();
	gdt.install();

	Terminal t;
	t.clear();
	t.print("Hello, Kernel!");

	while (1)
		;
}