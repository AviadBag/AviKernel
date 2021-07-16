#include "kernel/panic.h"

#include <cstdio.h>
#include <stdarg.h>

void panic(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    kprintf("------------ KERNEL PANIC ------------\n");
    vkprintf(format, args);

    va_end(args);

    while (true) { }
}
