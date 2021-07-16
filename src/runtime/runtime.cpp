#include <stddef.h>
#include <cstdlib.h>

#define MARKUSED(X) ((void)(&(X)))

void *operator new(size_t size)
{
    return kmalloc(size);
}
 
void *operator new[](size_t size)
{
    return kmalloc(size);
}
 
void operator delete(void *p)
{
    kfree(p);
}
 
void operator delete[](void *p)
{
    kfree(p);
}

/* Virtual function hundler */
extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}