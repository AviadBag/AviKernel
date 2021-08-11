#ifndef _CSTRING_H
#define _CSTRING_H

#include <stddef.h>

void* memset(void* s, int c, size_t n);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char *s);

#endif
