#include "kernel/mm/virtual_mgr/virtual_mgr.h"

// Extracts the page table index from a virtual address
#define VMMGR_GET_PAGE_TABLE_INDEX(v_address) ((((uint32_t) (v_address)) >> 12) & 0x3FF)

// Extracts the page directory index from a virtual address
#define VMMGR_GET_PAGE_DIRECTORY_INDEX(v_address) ((((uint32_t) (v_address)) >> 22) & 0x3FF)