#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>

class Heap
{
public:
    static void initialize();

    static void* kmalloc(size_t);
    static void kfree(void);
};

#endif // __HEAP_H__