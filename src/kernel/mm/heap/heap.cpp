#include "kernel/mm/heap/heap.h"

#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"

extern "C" uint32_t kernelEnd; // This variable sits at the end of the kernel.

heap_header* Heap::holes_list_head;

void Heap::initialize() 
{
    // kernelEnd is page aligned, and holes_list_head should also be page aligned..
    holes_list_head = (heap_header*) (&kernelEnd + VMMGR_PAGE_SIZE);
    physical_addr first_frame = PhysicalMgr::allocate_block();
    VirtualMgr::map(holes_list_head, first_frame, true);

    holes_list_head->size = PMMGR_PAGE_SIZE;
    holes_list_head->prev = nullptr;
    holes_list_head->next = nullptr;
}

void Heap::kfree(void) 
{

}

void* Heap::kmalloc(size_t) 
{
    return nullptr;
}