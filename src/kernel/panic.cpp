#include "kernel/panic.h"

#include <stdarg.h>
#include <cstdio.h>

void panic(const char* format, ...) 
{
    va_list args;
    va_start(args, format);

    printf("------------ KERNEL PANIC ------------\n");
    vprintf(format, args);

    va_end(args);

    while (true) {}
}
