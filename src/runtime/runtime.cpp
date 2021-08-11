#include <cstdlib.h>
#include <stddef.h>

#define MARKUSED(X) ((void)(&(X)))

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* p)
{
    free(p);
}

void operator delete[](void* p)
{
    free(p);
}

/* Virtual function hundler */
extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}