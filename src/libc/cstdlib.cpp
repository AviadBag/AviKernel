#include "kernel/mm/heap/heap.h"

#include <cstdlib.h>

char* itoa(int value, char* str, int base)
{
    char* rc;
    char* ptr;
    char* low;
    // Check for supported base.
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

void* malloc(size_t size)
{
    return Heap::malloc(size);
}

void free(void* addr)
{
    Heap::free(addr);
}

void* realloc(void *ptr, size_t new_size)
{
    return Heap::realloc(ptr, new_size);
}