#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/panic.h"
#include "utils/bitmap.h"

#include <cstdio.h>
#include <cstring.h>

#define PMMGR_USABLE_MEMORY_START 0x100000 // One MB. The lower memory (< 1MB) is reserved for hardware.
/**
 * @brief 4MB.
 * During early start up we set up a temporary virtual memory table. (See src/boot/boot.asm).
 * There we identity-map the first 4MB, and map the kernel. Therefore we can currently
 * only use the first 4MB and the kernel memory space.
 * We are still providing blocks on higher addressed, but they are to be used only after the
 * virtual memory manager is initialized.
 */
#define PMMGR_BOOT_MAPPING_MEMORY_END 0x400000

#define PMMGR_BITMAP_BLOCK_USED 1
#define PMMGR_BITMAP_BLOCK_FREE 0

// How many blocks does this chunk occupie (Rounded up)
#define PMMGR_BYTES_TO_BLOCKS(chunk) ((chunk) % PMMGR_BLOCK_SIZE == 0 ? (chunk) / PMMGR_BLOCK_SIZE : (chunk) / PMMGR_BLOCK_SIZE + 1)

// Convert an address to it's block number - Rounded down
#define PMMGR_ADDRESS_TO_BLOCK(address) ((address) / PMMGR_BLOCK_SIZE)

// The oppesite of PMMGR_ADDRESS_TO_BLOCK
#define PMMGR_BLOCK_TO_ADDRESS(address) ((address) * PMMGR_BLOCK_SIZE)

// The kernel is currently mapped virtually at 0xC0000000
#define PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(v) ((physical_addr)((uint32_t)(v) - 0xC0000000))

uint32_t* PhysicalMgr::bitmap;
uint64_t PhysicalMgr::number_of_blocks;
uint64_t PhysicalMgr::number_of_cells;
uint64_t PhysicalMgr::bitmap_size;

extern "C" void* kernelStart; // This variable sits at the beginning of the kernel.
extern "C" void* kernelEnd; // This variable sits at the end of the kernel.

void PhysicalMgr::initialize(uint32_t higher_memory_size, uint32_t mmap_addr, uint32_t mmap_length)
{
    printf("Initializing Physical Memory Manager...\n");

    // Init variables
    number_of_blocks = higher_memory_size / PMMGR_BLOCK_SIZE;
    number_of_cells = number_of_blocks / 32;
    bitmap_size = number_of_cells * 4; // Every cell is 32 bits, I need it in 8 bits units.

    find_memory_for_bitmap(mmap_addr, mmap_length);
    fill_bitmap(mmap_addr, mmap_length);
}

void PhysicalMgr::find_memory_for_bitmap(uint32_t mmap_addr, uint32_t mmap_length)
{
    multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmap_addr;
    while ((uint32_t)entry < (mmap_addr + mmap_length)) {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->addr >= PMMGR_USABLE_MEMORY_START && entry->addr + bitmap_size < PMMGR_BOOT_MAPPING_MEMORY_END) {
            if (entry->len >= bitmap_size) {
                // Make sure that we are not overriding the kernel
                uint64_t desired_bitmap_start_addr = entry->addr;
                uint64_t desired_bitmap_end_addr = desired_bitmap_start_addr + bitmap_size;
                uint64_t kernelStartP = (uint64_t)PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelStart);
                uint64_t kernelEndP = (uint64_t)PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelEnd);

                // Can the bitmap fit before the kernel?
                if (desired_bitmap_end_addr < kernelStartP) {
                    bitmap = (uint32_t*)desired_bitmap_start_addr;
                    return;
                }

                // Can the bitmap fit after the kernel?
                desired_bitmap_start_addr = kernelEndP + 1;
                desired_bitmap_end_addr = desired_bitmap_start_addr + bitmap_size;
                if (desired_bitmap_end_addr < entry->addr + entry->len) {
                    bitmap = (uint32_t*)desired_bitmap_start_addr;
                    return;
                }

                // We didn't find - try the next entry..
            }
        }

        // Go to next entry
        entry = (multiboot_memory_map_t*)((unsigned int)entry + entry->size + sizeof(entry->size));
    }

    panic("Physical Memory Manager: Not enough memory! Could not find enough place for the physical allocator bitmap");
}

void PhysicalMgr::fill_bitmap(uint32_t mmap_addr, uint32_t mmap_length)
{
    // Fill the bitmap with ones. (By default, all the blocks are used).
    memset(bitmap, 0, bitmap_size);

    // Iterate over the memory map to find the availabe memory regions.
    multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmap_addr;
    while ((uint32_t)entry < (mmap_addr + mmap_length)) {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
            // Mark this memory region as free
            mark_memory_as(entry->addr, entry->size, PMMGR_BITMAP_BLOCK_FREE);

        // Go to next entry
        entry = (multiboot_memory_map_t*)((unsigned int)entry + entry->size + sizeof(entry->size));
    }

    // Mark the first MB as used (Reserved for hardware)
    mark_memory_as(0, PMMGR_USABLE_MEMORY_START, PMMGR_BITMAP_BLOCK_USED);

    // Mark the bitmap as used
    mark_memory_as((uint32_t)bitmap, bitmap_size, PMMGR_BITMAP_BLOCK_USED);

    // Mark the kernel area as used
    uint32_t kernelStartP = (uint32_t)PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelStart);
    uint32_t kernelEndP = (uint32_t)PMMGR_VIRTUAL_TO_PHYSICAL_ADDR(&kernelEnd);
    mark_memory_as(kernelStartP, kernelEndP - kernelStartP, PMMGR_BITMAP_BLOCK_USED);
}

void PhysicalMgr::mark_memory_as(uint32_t start_address, size_t size, int what)
{
    size_t start_block = PMMGR_ADDRESS_TO_BLOCK(start_address);
    size_t end_block = PMMGR_ADDRESS_TO_BLOCK(start_address + size);
    size_t blocks_count = end_block - start_block + 1; // There is at least one block
    for (size_t i = 0; i < blocks_count; i++) {
        if (what == PMMGR_BITMAP_BLOCK_USED)
            Bitmap::set(bitmap, start_block + i);
        else
            Bitmap::clear(bitmap, start_block + i);
    }
}

physical_addr PhysicalMgr::allocate_block()
{
    // Iterate on each cell, and check where there is a free block.
    for (uint64_t i = 0; i < number_of_cells; i++) {
        uint32_t cell = bitmap[i];
        if (cell != 0xffffffff) // There is at least one zero here
        {
            for (int j = 0; j < 32; j++) {
                int bit = i * 32 + j;
                if (Bitmap::test(bitmap, bit) == PMMGR_BITMAP_BLOCK_FREE) {
                    // We have found a block!
                    Bitmap::set(bitmap, bit);
                    return (physical_addr)PMMGR_BLOCK_TO_ADDRESS(bit);
                }
            }
        }
    }

    return NULL;
}

void PhysicalMgr::free_block(physical_addr block)
{
    if ((uint32_t)block % PMMGR_BLOCK_SIZE != 0) // This is not page aligned!.
        return;

    size_t block_number = PMMGR_ADDRESS_TO_BLOCK((uint32_t)block);
    Bitmap::clear(bitmap, block_number);
}