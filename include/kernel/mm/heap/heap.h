#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include <stdint.h>

typedef struct heap_header_e
{
    size_t size;
    struct heap_header_e* next;
    struct heap_header_e* prev;
} heap_header;

class Heap
{
public:
    static void initialize();

    static void* kmalloc(size_t);
    static void kfree(void);

private:
    static bool extend_heap(size_t); // Extends the heap until there is enough space.
    // Splits the hole. Returns the size of the newly allocated memory. (Usually <size>, but can be more). Returns 0 if could not be splitted.
    static size_t split(heap_header*, size_t size);
    static void remove_from_holes_list(heap_header*);
    static void insert_to_holes_list(heap_header*);

private:
    static heap_header* holes_list_head;
    static heap_header* holes_list_tail;
    static void* last_page;
};

#endif // __HEAP_H__