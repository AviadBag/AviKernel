#include "kernel/panic.h"

#include <cstdio.h>
#include <stdarg.h>

void panic(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("------------ KERNEL PANIC (Surely Michael's fault) ------------\n");
    vprintf(format, args);

    va_end(args);

    while (true)
    {
    }
}
