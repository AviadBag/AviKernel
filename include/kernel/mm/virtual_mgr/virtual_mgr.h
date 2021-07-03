#ifndef __VIRTUAL_MGR_H__
#define __VIRTUAL_MGR_H__

#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/page_table_entry.h"
#include "kernel/mm/virtual_mgr/page_directory_entry.h"
#include "stdint.h"

typedef void* virtual_addr;

class VirtualMgr
{
    public:
        static void initialize();
        static void map(virtual_addr, physical_addr, bool requires_supervisor);

    private:
        static uint32_t page_directory[1024];
        static uint32_t page_etable[1024];

        // Converts a page table to it's actual address, given the index of the page table in the page directory.
        static virtual_addr get_page_table_virtual_address(int index);
};

#endif // __VIRTUAL_MGR_H__