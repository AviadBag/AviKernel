#include <drivers/terminal/terminal.h>
#include <cstdlib.h>

extern "C" void kernel_main(void) 
{
	Terminal t;
	t.clear();
	
	for (int i = 0; i < 5000; i++)
	{
		char str[] = {0, 0, 0, 0, 0};
		itoa(i, str, 10);
		t.print(str);
		t.print("\n");
	}
}