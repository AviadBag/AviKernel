#ifndef __VIRTUAL_MGR_H__
#define __VIRTUAL_MGR_H__

#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/page_directory_entry.h"
#include "kernel/mm/virtual_mgr/page_table_entry.h"
#include "stdint.h"

#define VMMGR_PAGE_SIZE 4096

typedef void* virtual_addr;

class VirtualMgr {
public:
    /**
     * @brief Initializes the Virtual Memory Manager.
     * 
     */
    static void initialize();

    /**
     * @brief Maps the given physical frame into the given virutal page.
     * 
     * @param v_addr The page to map to. (MUST BE PAGE ALIGNED!)
     * @param p_addr The frame to map. (MUST BE PAGE ALIGNED!)
     * @param requires_supervisor Do the pages require a supervisor?
     */
    static void map(virtual_addr v_addr, physical_addr p_addr, bool requires_supervisor);

private:
    static uint32_t page_directory[1024] __attribute__((aligned(VMMGR_PAGE_SIZE)));
    static uint32_t page_etable[1024] __attribute__((aligned(VMMGR_PAGE_SIZE)));

    // Converts a page table to it's actual address, given the index of the page table in the page directory.
    static virtual_addr get_page_table_virtual_address(int index);

    // Maps the next <count> pages.
    /**
     * @brief Maps the next <count> pages.
     * 
     * @param v_addr The virtual address of the page to start mapping to.
     * @param p_addr The physical address to start mapping from.
     * @param count  How many pages do you want to map?
     * @param requires_supervisor Do the pages require a supervisor?
     */
    static void map_range(virtual_addr v_addr, physical_addr p_addr, size_t count, bool requires_supervisor);

    /**
     * Puts the page_etable in the CURRENT page directory (pointed to by CR3).
    */
    static void put_page_etable();

    static inline void invalidate(virtual_addr);
};

#endif // __VIRTUAL_MGR_H__