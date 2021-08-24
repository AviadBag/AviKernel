#ifndef _CSTDLIB_H
#define _CSTDLIB_H
#include <stddef.h>

char* itoa(int value, char* str, int base);

void* malloc(size_t);
void free(void*);
void* realloc(void* ptr, size_t new_size);

#endif