#include <size_t.h>

/* Memory manegmnet functions */
void *operator new(size_t size)
{
    /* Nothing here in the meantime */
    return (void*)(0);
}
 
void *operator new[](size_t size)
{
    /* Nothing here in the meantime */
    return (void*)(0);
}
 
void operator delete(void *p)
{
    /* Nothing here in the meantime */
}
 
void operator delete[](void *p)
{
    /* Nothing here in the meantime */
}


/* Virtual function hundler */
extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}