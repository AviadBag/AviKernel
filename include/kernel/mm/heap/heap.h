#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include <stdint.h>

typedef struct heap_header_e
{
    uint32_t size;
    struct heap_header_e* next;
    struct heap_header_e* prev;
} heap_header;

class Heap
{
public:
    static void initialize();

    static void* kmalloc(size_t);
    static void kfree(void);
};

#endif // __HEAP_H__