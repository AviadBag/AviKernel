#include <kernel/terminal/terminal.h>

extern "C" void kernel_main(void) 
{
	Terminal t;
	t.clear();
	t.print("Hello from the kernel!\n");
}