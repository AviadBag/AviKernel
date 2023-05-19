#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include <stdint.h>

typedef struct heap_hole_header_s
{
    size_t size;                     // The size of the hole, INCLUDING THE HEADER.
    struct heap_hole_header_s *next; // A pointer to the next hole on the list.
    struct heap_hole_header_s *prev; // A pointer to the previous hole on the list.
} heap_hole_header;

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
    /**
     * @brief Extends the heap until it containes at least <size> of free space.
     *
     * @param size The minimum size to add.
     * @return true On success.
     * @return false If there is no enough memory for that.
     */
    static bool extend_heap(size_t size);

    /**
     * @brief Splits the given hole into one allocated space and one hole.
     *
     * @param hole Into one allocates space and one hole.
     * @param size The allocates space required size.
     * @return size_t The size of the first hole. (Usually <size>, but can be more). Returns 0 if could not be splitted.
     */
    static size_t split(heap_hole_header *hole, size_t size);

    /**
     * @brief Merges given hole with previous and next holes if possible.
     *        That is in oreder to transfer a few holes in a row into one big hole.
     *        If previous hole or next hole are not right next to the given hole then we won't be able to merge.
     *
     * @param hole The hole to merge.
     */
    static void merge(heap_hole_header *hole);

    // Helper functions for merge(). They are not checking for possibility.
    static void merge_previous(heap_hole_header *);
    static void merge_next(heap_hole_header *);

    /**
     * @brief Removes given hole from the global holes list.
     *
     * @param hole The hole to remove.
     */
    static void remove_from_holes_list(heap_hole_header *hole);

    /**
     * @brief Inserts given hole to the global holes list, in it's right place.
     *
     * @param hole The hole to insert.
     */
    static void insert_to_holes_list(heap_hole_header *hole);

private:
    static heap_hole_header *holes_list_head; // The head of the doubly linked list.
    static heap_hole_header *holes_list_tail; // The tail of the doubly linked list.
    static void *last_page;                   // A pointer to the last page on the heap.
};

#endif // __HEAP_H__