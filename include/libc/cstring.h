#ifndef _CSTRING_H
#define _CSTRING_H

#include <stddef.h>

void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);

int strcmp(const char* s1, const char* s2);
size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);

#endif
