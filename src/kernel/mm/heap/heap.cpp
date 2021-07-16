#include "kernel/mm/heap/heap.h"

#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include <cstdio.h>

// Adds <amount> to the void pointer <void_p>
#define HEAP_ADD_TO_VOID_P(void_p, amount) ((void_p) = (void*) ( ((char*)(void_p)) + amount ))

// Subtracts <amount> from the void pointer <void_p>
#define HEAP_SUBTRACT_FROM_VOID_P(void_p, amount) ((void_p) = (void*) ( ((char*)(void_p)) - amount ))

extern "C" uint32_t kernelEnd; // This variable sits at the end of the kernel.

heap_header* Heap::holes_list_head;
heap_header* Heap::holes_list_tail;
void*        Heap::last_page;

void Heap::initialize() 
{
    // kernelEnd is page aligned, and holes_list_head should also be page aligned..
    holes_list_head = holes_list_tail = (heap_header*) (&kernelEnd + VMMGR_PAGE_SIZE);
    physical_addr first_frame = PhysicalMgr::allocate_block();
    if (!first_frame)
        panic("Not enough space for heap!");
    
    VirtualMgr::map(holes_list_head, first_frame, true);
    last_page = holes_list_head;

    holes_list_head->size = PMMGR_PAGE_SIZE;
    holes_list_head->prev = NULL;
    holes_list_head->next = NULL;
}

bool Heap::extend_heap(size_t size) 
{
    size_t desired_size = size + sizeof(heap_header); // How many size we REALLY nead
    size_t desired_pages = desired_size / VMMGR_PAGE_SIZE + 1;

    heap_header* header = NULL;
    HEAP_ADD_TO_VOID_P(last_page, VMMGR_PAGE_SIZE);

    bool status = true;

    size_t number_of_pages_done;
    for (number_of_pages_done = 0; number_of_pages_done < desired_pages; number_of_pages_done++, HEAP_ADD_TO_VOID_P(last_page, VMMGR_PAGE_SIZE))
    {
        physical_addr frame = PhysicalMgr::allocate_block();
        if (!frame) 
        {
            status = false;
            break;
        }

        VirtualMgr::map(last_page, frame, true);
        if (header == NULL) // Do it only at the first time
            header = (heap_header*) last_page;
    }

    // Fill header
    header->size = number_of_pages_done * VMMGR_PAGE_SIZE;
    header->next = NULL;
    header->prev = holes_list_tail;

    insert_to_holes_list(header);

    return status;
}

void* Heap::kmalloc(size_t size)
{
    if (size == 0) return NULL;

    size_t required_size = size + sizeof(size_t); // I also have to store the amount of allocated bytes.

    // Find the first big enough hole
    heap_header* node = holes_list_head;
    heap_header* hole = NULL;
    while (node != NULL)
    {
        if (node->size >= required_size)
        {
            hole = node;
            break;
        }

        node = node->next;
    }

    if (hole == NULL) // Not found...
    {
        if (!extend_heap(required_size))
            return NULL;

        return kmalloc(size); // Try again
    }

    size_t allocated_size = split(hole, required_size);
    if (allocated_size == 0) // Was not splitted
    {
        allocated_size = hole->size; // It can take the whole size
        remove_from_holes_list(hole);
    }

    *((size_t*) hole) = allocated_size;
    void* addr = (void*) ((size_t) hole + sizeof(size_t)); // Skip to after the bytes that represent the allocated size
    
    return addr;
}

size_t Heap::split(heap_header* hole, size_t size)
{
    if (hole->size - size > sizeof(heap_header)) // Can it be splitted?
    {
        size_t offset = size > sizeof(heap_header) ? size : sizeof(heap_header);
        heap_header* new_hole = (heap_header*) ((size_t) hole + offset);
        new_hole->size = hole->size - offset;
        remove_from_holes_list(hole);
        insert_to_holes_list(new_hole);

        return offset;
    }
    return 0;
}

void Heap::remove_from_holes_list(heap_header* hole) 
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
        hole->prev->next = hole->next; // Skip me!
}

void Heap::insert_to_holes_list(heap_header* hole)
{
    if (holes_list_tail == NULL) // The list is empty
    {
        holes_list_tail = holes_list_head = hole;
        hole->next = NULL;
        hole->prev = NULL;
    }
    else
    {
        hole->prev = holes_list_tail;
        holes_list_tail->next = hole;
        holes_list_tail = hole;
    }
}

void Heap::kfree(void* addr) 
{
    HEAP_SUBTRACT_FROM_VOID_P(addr, sizeof(size_t)); // Go to the actual beginning of the allocated space.
    size_t how_many_bytes = *(size_t*)(addr);

    heap_header* header = (heap_header*) addr;
    header->size = how_many_bytes;
    insert_to_holes_list(header);
}