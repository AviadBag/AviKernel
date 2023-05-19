#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include <stdint.h>

typedef struct heap_header_e
{
    size_t size;
    struct heap_header_e *next;
    struct heap_header_e *prev;
} heap_header;

class Heap
{
public:
    /**
     * @brief Initializes the Heap.
     */
    static void initialize();

    /**
     * @brief Allocates a chunk of memory on the heap.
     *
     * @param size The required size for the chunk.
     * @return void* A pointer to the allocated memory.
     */
    static void *malloc(size_t size);

    /**
     * @brief Frees the memory chunk pointed by <ptr>. The memory must have been previously
     *        allocated by malloc(). Unexpected results if not.
     *
     * @param ptr A pointer to the memory chunk.
     */
    static void free(void *ptr);

    /**
     * @brief Changes the size of the given chunk. Data will remain unchanged.
     *
     * @param ptr      A pointer to the memory chunk to resize.
     * @param new_size The new required size.
     * @return void*
     */
    static void *realloc(void *ptr, size_t new_size);

private:
    static bool extend_heap(size_t); // Extends the heap until there is enough space.
    // Splits the given hole into one allocated space and one hole.
    // Returns the size of the first hole. (Usually <size>, but can be more). Returns 0 if could not be splitted.
    static size_t split(heap_header *, size_t size);
    static void merge(heap_header *); // Merges the previous hole and next hole, IF POSSIBLE.
    static void merge_previous(heap_header *);
    static void merge_next(heap_header *);
    static void remove_from_holes_list(heap_header *);
    static void insert_to_holes_list(heap_header *);

private:
    static heap_header *holes_list_head;
    static heap_header *holes_list_tail;
    static void *last_page;
};

#endif // __HEAP_H__