#ifndef _PHYSICAL_MGR
#define _PHYSICAL_MGR

#include "multiboot/multiboot.h"

#include <stddef.h>
#include <stdint.h>

#define PMMGR_PAGE_SIZE (1024 * 4) // 4 KB

typedef void* physical_addr;

class PhysicalMgr {
public:
    // higher_memory_size is in bytes!
    static void initialize(uint32_t higher_memory_size, uint32_t mmap_addr, uint32_t mmap_length);

    static physical_addr allocate_block();
    // Must be a block address. If it is not - nothing will happen.
    static void free_block(physical_addr block);

private:
    // Finds a memory location for the bitmap
    static void find_memory_for_bitmap(uint32_t mmap_addr, uint32_t mmap_length);
    static void fill_bitmap(uint32_t mmap_addr, uint32_t mmap_length);

private:
    // "what" is PMMGR_BITMAP_PAGE_USED or PMMGR_BITMAP_PAGE_FREE.
    // "size" = how many bytes.
    // size must be > 0!
    static void mark_memory_as(uint32_t start_address, size_t size, int what);

    static uint32_t* bitmap; // A clear bit means a free page.
    static uint64_t number_of_pages;
    static uint64_t number_of_cells;
    static uint64_t bitmap_size;
};

#endif