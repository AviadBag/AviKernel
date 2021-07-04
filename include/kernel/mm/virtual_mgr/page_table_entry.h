#include <stdint.h>

#include "kernel/mm/physical_mgr/physical_mgr.h" // For physiacl_addr
#include "kernel/mm/virtual_mgr/paging_entry.h"

#ifndef _PAGE_TABLE_ENTRY_H
#define _PAGE_TABLE_ENTRY_H

class PageTableEntry : public PagingEntry
{
public:
    PageTableEntry();
    PageTableEntry(bool present, bool writeable, bool requires_supervisor, physical_addr frame);

    bool get_dirty();  // Dirty flag

    static PageTableEntry from_bytes(uint32_t);
};

#endif