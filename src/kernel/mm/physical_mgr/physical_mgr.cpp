#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/panic.h"
#include "utils/bitmap.h"

#include <cstdio.h>
#include <cstring.h>

#define PMMGR_USABLE_MEMORY_START 0x100000 // One MB
#define PMMGR_MAPPED_MEMORY_END   0x400000 // 4 MB

#define PMMGR_BITMAP_PAGE_USED 1
#define PMMGR_BITMAP_PAGE_FREE 0

// How many pages does this chunk occupie (Rounded up)
#define PMMGR_BYTES_TO_PAGES(chunk) ((chunk) % PMMGR_PAGE_SIZE == 0 ? (chunk) / PMMGR_PAGE_SIZE : (chunk) / PMMGR_PAGE_SIZE + 1)

// Convert an address to it's page number - Rounded down
#define PMMGR_ADDRESS_TO_PAGE(address) ((address) / PMMGR_PAGE_SIZE)

// The oppesite of PMMGR_ADDRESS_TO_PAGE
#define PMMGR_PAGE_TO_ADDRESS(address) ((address)*PMMGR_PAGE_SIZE)

#define PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(v) ((physical_addr) ((uint32_t) (v) - 0xC0000000))

uint32_t *PhysicalMgr::bitmap;
uint64_t PhysicalMgr::number_of_pages;
uint64_t PhysicalMgr::number_of_cells;
uint64_t PhysicalMgr::bitmap_size;

extern "C" void *kernelStart; // This variable sits at the beginning of the kernel.
extern "C" void *kernelEnd;   // This variable sits at the end of the kernel.

void PhysicalMgr::initialize(uint32_t higher_memory_size, uint32_t mmap_addr, uint32_t mmap_length)
{
    // Init variables
    number_of_pages = higher_memory_size / PMMGR_PAGE_SIZE; // higher_memory_size
    number_of_cells = number_of_pages / 32;
    bitmap_size = number_of_cells * 4; // Every cell is 32 bits, I need it in 8 bits units.

    find_memory_for_bitmap(mmap_addr, mmap_length);
    fill_bitmap(mmap_addr, mmap_length);
}

void PhysicalMgr::mark_memory_as(uint32_t start_address, size_t size, int what)
{
    size_t start_page = PMMGR_ADDRESS_TO_PAGE(start_address);
    size_t end_page = PMMGR_ADDRESS_TO_PAGE(start_address + size);
    size_t pages_count = end_page - start_page + 1; // There is at least one page
    for (size_t i = 0; i < pages_count; i++)
    {
        if (what == PMMGR_BITMAP_PAGE_USED)
            Bitmap::set(bitmap, start_page + i);
        else
            Bitmap::clear(bitmap, start_page + i);
    }
}

void PhysicalMgr::fill_bitmap(uint32_t mmap_addr, uint32_t mmap_length)
{
    // Fill the bitmap with ones. (By default, all the pages are used).
    mark_memory_as(0, number_of_pages-1, PMMGR_BITMAP_PAGE_USED);

    multiboot_memory_map_t *entry = (multiboot_memory_map_t *)mmap_addr;
    while ((uint32_t)entry < (mmap_addr + mmap_length))
    {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
            // Mark this memory region as free
            mark_memory_as(entry->addr, entry->size, PMMGR_BITMAP_PAGE_FREE);

        // Go to next entry
        entry = (multiboot_memory_map_t *)((unsigned int)entry + entry->size + sizeof(entry->size));
    }

    // Mark the first MB as used
    mark_memory_as(0, PMMGR_USABLE_MEMORY_START, PMMGR_BITMAP_PAGE_USED);

    // Mark the bitmap as used
    mark_memory_as((uint32_t)bitmap, bitmap_size, PMMGR_BITMAP_PAGE_USED);

    // Mark the kernel area as used
    uint32_t kernelStartP = (uint32_t) PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelStart);
    uint32_t kernelEndP = (uint32_t) PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelEnd);
    mark_memory_as(kernelStartP, kernelEndP - kernelStartP, PMMGR_BITMAP_PAGE_USED);
    kprintf("Kernel start: 0x%x, Kernel end: 0x%x\n", kernelStartP, kernelEndP);
}

void PhysicalMgr::find_memory_for_bitmap(uint32_t mmap_addr, uint32_t mmap_length)
{
    multiboot_memory_map_t *entry = (multiboot_memory_map_t *)mmap_addr;
    kprintf("Memory Map:\n");
    while ((uint32_t)entry < (mmap_addr + mmap_length))
    {
        kprintf("Base address: 0x%p", entry->addr);
        kprintf(", End address: 0x%p", entry->addr + entry->len);
        kprintf(", Entry Type: %x\n", entry->type);

        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->addr >= PMMGR_USABLE_MEMORY_START && entry->addr + bitmap_size < PMMGR_MAPPED_MEMORY_END)
        {
            if (entry->len >= bitmap_size)
            {
                // Make sure that we are not overriding the kernel
                uint64_t desired_bitmap_start_addr = entry->addr;
                uint64_t desired_bitmap_end_addr = desired_bitmap_start_addr + bitmap_size;
                uint64_t kernelStartP = (uint64_t) PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelStart);
                uint64_t kernelEndP = (uint64_t) PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelEnd);

                // Can the bitmap fit before the kernel?
                if (desired_bitmap_end_addr < kernelStartP)
                {
                    bitmap = (uint32_t *)desired_bitmap_start_addr;
                    kprintf("Placing bitmap before kernel, on address 0x%x", desired_bitmap_start_addr);
                    kprintf(", Bitmap end = 0x%x\n", desired_bitmap_end_addr);
                    return;
                }
                
                // Can the bitmap fit after the kernel?
                desired_bitmap_start_addr = kernelEndP + 1;
                desired_bitmap_end_addr = desired_bitmap_start_addr + bitmap_size;
                if (desired_bitmap_end_addr < entry->addr + entry->len)
                {
                    bitmap = (uint32_t*)desired_bitmap_start_addr;
                    kprintf("Placing bitmap after kernel, on address 0x%x", desired_bitmap_start_addr);
                    kprintf(", Bitmap end = 0x%x\n", desired_bitmap_end_addr);
                    return;
                }

                // We didn't find - try the next entry..
            }
        }

        // Go to next entry
        entry = (multiboot_memory_map_t *)((unsigned int)entry + entry->size + sizeof(entry->size));
    }

    panic("Physical Memory Manager: Not enough memory! Could not find enough place for the physical allocator bitmap");
}

physical_addr PhysicalMgr::allocate_block()
{
    // Iterate on each cell, and check where there is a free block.
    for (uint64_t i = 0; i < number_of_cells; i++)
    {
        uint32_t cell = bitmap[i];
        if (cell != 0xffffffff) // There is at least one zero here
        {
            for (int j = 0; j < 32; j++)
            {
                int cell_number = i * 32 + j;
                if (Bitmap::test(bitmap, cell_number) == PMMGR_BITMAP_PAGE_FREE)
                {
                    // We have found a block!
                    Bitmap::set(bitmap, cell_number);
                    return (physical_addr)PMMGR_PAGE_TO_ADDRESS(cell_number);
                }
            }
        }
    }

    return NULL;
}

void PhysicalMgr::free_block(physical_addr block)
{
    if ((uint32_t) block % PMMGR_PAGE_SIZE != 0) // This is not a page size.
        return;

    size_t page_number = PMMGR_ADDRESS_TO_PAGE((uint32_t) block);
    Bitmap::clear(bitmap, page_number);
}