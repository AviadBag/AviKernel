#include <stddef.h>

#define MARKUSED(X)  ((void)(&(X)))

/* Memory manegmnet functions */
void *operator new(size_t size)
{
    /* Nothing here in the meantime */
    MARKUSED(size);
    return (void*)(0);
}
 
void *operator new[](size_t size)
{
    /* Nothing here in the meantime */
    MARKUSED(size);
    return (void*)(0);
}
 
void operator delete(void *p)
{
    /* Nothing here in the meantime */
    MARKUSED(p);
}
 
void operator delete[](void *p)
{
    /* Nothing here in the meantime */
    MARKUSED(p);
}


/* Virtual function hundler */
extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}