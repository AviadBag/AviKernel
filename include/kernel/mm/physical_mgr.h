#ifndef _PHYSICAL_MGR
#define _PHYSICAL_MGR

#include "multiboot/multiboot.h"

#include <cstdint>

typedef void* physical_addr;

class PhysicalMgr {
public:
    static void initialize(uint32_t mmap_addr, uint32_t mmap_length);

    static physical_addr allocate_block();
    static void free_block(physical_addr);
};

#endif