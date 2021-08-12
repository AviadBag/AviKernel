#include <cstring.h>

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *)s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

size_t strlen(const char *str)
{
    const char *s;
    for (s = str; *s; ++s)
        ;
    return (s - str);
}

// Taken from https://www.tutorialspoint.com/write-your-own-memcpy-in-c
void *memcpy(void *dest, const void *src, size_t n)
{
    //cast src and dest to char*
    char *src_char = (char *)src;
    char *dest_char = (char *)dest;
    for (size_t i = 0; i < n; i++)
        dest_char[i] = src_char[i]; //copy contents byte by byte

    return dest;
}