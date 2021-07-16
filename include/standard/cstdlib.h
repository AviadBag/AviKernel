#ifndef _CSTDLIB_H
#define _CSTDLIB_H
#include <stddef.h>

char* itoa(int value, char* str, int base);

void* kmalloc(size_t);
void kfree(void*);

#endif