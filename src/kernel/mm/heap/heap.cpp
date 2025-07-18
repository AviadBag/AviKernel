#include "kernel/mm/heap/heap.h"

#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include <cstdio.h>
#include <cstring.h>

#define HEAP_START_ADDR 0xC0C00000

// Adds <amount> to the void pointer <void_p>
#define HEAP_ADD_TO_VOID_P(void_p, amount) ((void_p) = (void *)(((char *)(void_p)) + amount))

// Subtracts <amount> from the void pointer <void_p>
#define HEAP_SUBTRACT_FROM_VOID_P(void_p, amount) ((void_p) = (void *)(((char *)(void_p)) - amount))

extern "C" uint32_t kernelEnd; // This variable sits at the end of the kernel.

heap_hole_header *Heap::holes_list_head;
heap_hole_header *Heap::holes_list_tail;
void *Heap::last_page;

void Heap::initialize()
{
    printf("Initializing Heap...\n");

    // Set the beginning of the memory right after the video memory. Should be page aligned.
    // See virtual_mgr.avidocs for virtual memory map.
    holes_list_head = holes_list_tail = (heap_hole_header *)(HEAP_START_ADDR);
    physical_addr first_frame = PhysicalMgr::allocate_block();
    if (!first_frame)
        panic("Not enough space for heap!");

    VirtualMgr::map(holes_list_head, first_frame, true);
    last_page = holes_list_head;

    // Set our first hole.
    holes_list_head->size = PMMGR_BLOCK_SIZE;
    holes_list_head->prev = NULL;
    holes_list_head->next = NULL;
}

bool Heap::extend_heap(size_t size)
{
    size_t desired_size = size + sizeof(heap_hole_header); // What size we REALLY need
    size_t desired_pages = desired_size / VMMGR_PAGE_SIZE + 1;

    heap_hole_header *header = NULL;
    HEAP_ADD_TO_VOID_P(last_page, VMMGR_PAGE_SIZE); // Get to the END of the heap.

    bool status = true;

    size_t number_of_pages_done;
    for (number_of_pages_done = 0; number_of_pages_done < desired_pages; number_of_pages_done++, HEAP_ADD_TO_VOID_P(last_page, VMMGR_PAGE_SIZE))
    {
        physical_addr frame = PhysicalMgr::allocate_block();
        if (!frame)
        {
            // Oh no, not enough memory.
            status = false;
            break;
        }

        VirtualMgr::map(last_page, frame, true);
        if (header == NULL) // Do it only at the first time
            header = (heap_hole_header *)last_page;
    }

    // Fill header
    header->size = number_of_pages_done * VMMGR_PAGE_SIZE;
    insert_to_holes_list(header);

    return status;
}

void *Heap::malloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t required_size = size + sizeof(size_t); // Include the header

    // Find the first big enough hole
    heap_hole_header *node = holes_list_head;
    heap_hole_header *hole = NULL;
    while (node != NULL)
    {
        if (node->size >= required_size)
        {
            // Yay we found one!
            hole = node;
            break;
        }

        node = node->next;
    }

    if (hole == NULL) // We haven't found any hole..
    {
        // Try extending the heap
        if (!extend_heap(required_size))
            // Not enough memory.
            return NULL;

        return malloc(size); // Try again
    }

    size_t allocated_size = split(hole, required_size);
    if (allocated_size == 0) // Was not splitted
    {
        allocated_size = hole->size;  // It can take the whole size
        remove_from_holes_list(hole); // Well, it is allocated now..
    }

    *((size_t *)hole) = allocated_size;                   // We can override the header. It is not used anymore, not until freed.
    void *addr = (void *)((size_t)hole + sizeof(size_t)); // Skip to after the bytes that represent the allocated size

    return addr;
}

size_t Heap::split(heap_hole_header *hole, size_t size)
{
    if (hole->size - size > sizeof(heap_hole_header)) // Can it be splitted?
    {
        // Where to split? (We must preserve space for the heap header, so we will be able to fill it when free)
        size_t offset = size > sizeof(heap_hole_header) ? size : sizeof(heap_hole_header);
        heap_hole_header *new_hole = (heap_hole_header *)((char *)hole + offset); // Here will be the header of the new hole

        // Fill data about the new hole
        new_hole->size = hole->size - offset;
        insert_to_holes_list(new_hole);

        // Mark the old hole as allocated
        remove_from_holes_list(hole);

        return offset;
    }
    return 0;
}

void Heap::remove_from_holes_list(heap_hole_header *hole)
{
    // Case 1 - This is the only hole
    if (hole->prev == NULL && hole->next == NULL)
        holes_list_tail = holes_list_head = NULL;
    // Case 2 - This is the first hole
    else if (hole == holes_list_head)
    {
        holes_list_head = holes_list_head->next;
        holes_list_head->prev = NULL;
    }
    // Case 3 - This is the last hole
    else if (hole == holes_list_tail)
    {
        holes_list_tail = holes_list_tail->prev;
        holes_list_tail->next = NULL;
    }
    // Case 4 - This is a regular hole (has next and prev)
    else
    {
        // Skip me!
        hole->prev->next = hole->next;
        hole->next->prev = hole->prev;
    }
}

void Heap::insert_to_holes_list(heap_hole_header *hole)
{
    if (holes_list_tail == NULL) // The list is empty
    {
        holes_list_tail = holes_list_head = hole;
        hole->next = NULL;
        hole->prev = NULL;
    }
    else
    {
        // Find the right place.
        // At the end of the loop, <node> will contain the node that should be AFTER <hole>
        heap_hole_header *node = holes_list_head;
        while (node != NULL)
        {
            if (hole < node)
                break;

            node = node->next;
        }

        // Case 1: That should be the first
        if (node == holes_list_head)
        {
            holes_list_head = hole;
            hole->prev = NULL;
            hole->next = node;
            node->prev = hole;
        }
        // Case 2: That should be the last
        else if (node == NULL)
        {
            holes_list_tail->next = hole;
            hole->prev = holes_list_tail;
            holes_list_tail = hole;
            hole->next = NULL;
        }
        // Case 3: That should be put in the middle
        else
        {
            heap_hole_header *prev = node->prev;

            // Bind to prev
            prev->next = hole;
            hole->prev = prev;

            // Bind to next
            node->prev = hole;
            hole->next = node;
        }
    }
}

void Heap::merge_next(heap_hole_header *header)
{
    header->size += header->next->size;
    remove_from_holes_list(header->next);
}

void Heap::merge_previous(heap_hole_header *header)
{
    header->prev->size += header->size;
    remove_from_holes_list(header);
}

void Heap::merge(heap_hole_header *header)
{
    if ((char *)header->prev + header->prev->size == (char *)header)
        merge_previous(header);
    if ((char *)header + header->size == (char *)header->next)
        merge_next(header);
}

void Heap::free(void *addr)
{
    /* Warning: When using this code in other places, do "- sizeof(size_t)", because the size indicator counts itself
    as well! */
    HEAP_SUBTRACT_FROM_VOID_P(addr, sizeof(size_t)); // Go to the actual beginning of the allocated space.
    size_t how_many_bytes = *(size_t *)(addr);       // At the beginning of every allocated chunk there is a size_t indicating the size of this chunk.

    heap_hole_header *header = (heap_hole_header *)addr; // Now that it's free we should populate it with a header.
    header->size = how_many_bytes;
    insert_to_holes_list(header);

    merge(header);
}

void *Heap::realloc(void *ptr, size_t new_size)
{
    if (!ptr)
        return malloc(new_size);
    else if (new_size == 0)
    {
        free(ptr);
        return nullptr;
    }

    void *tmp = ptr;
    HEAP_SUBTRACT_FROM_VOID_P(tmp, sizeof(size_t));
    size_t old_size = *(size_t *)(tmp) - sizeof(size_t); // The size indicator counts itself as well.

    void *new_place = malloc(new_size);
    if (!new_place)
        return nullptr;

    size_t bytes_to_copy = (new_size > old_size) ? old_size : new_size;
    memcpy(new_place, ptr, bytes_to_copy);
    free(ptr);

    return new_place;
}
