#include "kernel/mm/virtual_mgr/page_table_entry.h"

#include "utils/bitmap.h"

enum PTE_FLAGS_INDEXES
{
    PTE_FLAGS_DIRTY = 6
};

PageTableEntry::PageTableEntry() : PagingEntry() {}

PageTableEntry::PageTableEntry(bool present, bool writeable, bool requires_supervisor, physical_addr frame) : PagingEntry(present, writeable, requires_supervisor, frame)
{
}

bool PageTableEntry::get_dirty()
{
    return Bitmap::test(&entry, PTE_FLAGS_DIRTY);
}

PageTableEntry PageTableEntry::from_bytes(uint32_t bytes)
{
    PageTableEntry e;
    e.entry = bytes;
    return e;
}