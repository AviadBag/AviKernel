#include "kernel/mm/virtual_mgr/page_table_entry.h"

#include "utils/bitmap.h"

enum PTE_FLAGS_INDEXES
{
    PTE_FLAGS_DIRTY = 6
};

#define PGT_FRAME_MASK 0xFFFFF000

PageTableEntry::PageTableEntry() : PagingEntry() {}

bool PageTableEntry::get_dirty()
{
    return Bitmap::test(&entry, PTE_FLAGS_DIRTY);
}