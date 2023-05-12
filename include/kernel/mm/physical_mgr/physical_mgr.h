#ifndef _PHYSICAL_MGR
#define _PHYSICAL_MGR

#include "multiboot/multiboot.h"

#include <stddef.h>
#include <stdint.h>

#define PMMGR_BLOCK_SIZE (1024 * 4) // 4 KB

typedef void *physical_addr;

class PhysicalMgr
{
public:
    /**
     * @brief the Physical Memory Manager.
     *
     * @param higher_memory_size Should be in bytes. The PMMGR does not use the lower memory.
     * @param mmap_addr          The memory map, according to multiboot specification.
     * @param mmap_length        The length of the memory map.
     */
    static void initialize(uint32_t higher_memory_size, uint32_t mmap_addr, uint32_t mmap_length);

    /**
     * @brief The main function of the PMMGR - allocates you a physical block somewhere.
     *        The given block hes the size of PMMGR_BLOCK_SIZE.
     *
     * @return physical_addr
     */
    static physical_addr allocate_block();

    /**
     * @brief Frees the given block for later re-use.
     *
     * @param block The physical address of the block. If the address is invalid then nothing will happen.
     */
    static void free_block(physical_addr block);

private:
    /**
     * @brief Finds in the given multiboot memry map a suitable place to put our bitmap in.
     *
     * @param mmap_addr   The memory map, according to multiboot specification.
     * @param mmap_length The length of the memory map.
     */
    static void find_memory_for_bitmap(uint32_t mmap_addr, uint32_t mmap_length);

    /**
     * @brief Fills the bitmap with initial information.
     *
     * @param mmap_addr   The memory map, according to multiboot specification.
     * @param mmap_length The length of the memory map.
     */
    static void fill_bitmap(uint32_t mmap_addr, uint32_t mmap_length);

    /**
     * @brief Marks the given memory region as either used or free.
     *        This region can be anything, it does not depend on blocks.
     *
     * @param start_address The beginning address of the memory aregion
     * @param size          The size of the memory region
     * @param what          Either PMMGR_BITMAP_BLOCK_USED or PMMGR_BITMAP_BLOCK_FREE.
     */
    static void mark_memory_as(uint32_t start_address, size_t size, int what);

    /**
     * In this bitmap we store the state of every block on the system.
     * A clear bit represents a free block, a set bit is a block in use.
     */
    static uint32_t *bitmap;

    static uint64_t number_of_cells;  // How many cells do we need in the bitmap? (Cell = 32bit)
    static uint64_t bitmap_size;      // The size of the bitmap (in bytes).
    static uint64_t number_of_blocks; // The total number of the blocks we have on the system
};

#endif