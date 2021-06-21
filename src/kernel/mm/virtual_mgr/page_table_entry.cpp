#include "kernel/mm/virtual_mgr/page_table_entry.h"

#include "utils/bitmap.h"

enum PGT_FLAGS_INDEXES
{
    PGT_FLAGS_PRESENT = 0,
    PGT_FLAGS_WRITABLE = 1,
    PGT_FLAGS_REQUIRES_SUPERVISOR = 2,
    PGT_FLAGS_ACCESSED = 5,
    PGT_FLAGS_DIRTY = 6
};

#define PGT_FRAME_MASK 0xFFFFF000

PageTableEntry::PageTableEntry() : entry(0)
{
}

uint32_t PageTableEntry::to_bytes()
{
    return entry;
}

void PageTableEntry::set_frame(physical_addr addr)
{
    entry &= ~PGT_FRAME_MASK; // Make the entry's address zero.
    uint32_t addr_p = (uint32_t)addr;
    addr_p &= PGT_FRAME_MASK;

    entry |= addr_p;
}

void PageTableEntry::set_present(bool p)
{
    Bitmap::put(&entry, PGT_FLAGS_PRESENT, p);
}

void PageTableEntry::set_writeable(bool w)
{
    Bitmap::put(&entry, PGT_FLAGS_WRITABLE, w);
}

void PageTableEntry::set_requires_supervisor(bool r)
{
    Bitmap::put(&entry, PGT_FLAGS_REQUIRES_SUPERVISOR, r);
}

bool PageTableEntry::get_present()
{
    return Bitmap::test(&entry, PGT_FLAGS_PRESENT);
}

bool PageTableEntry::get_writeable()
{
    return Bitmap::test(&entry, PGT_FLAGS_WRITABLE);
}

bool PageTableEntry::get_requires_supervisor()
{
    return Bitmap::test(&entry, PGT_FLAGS_REQUIRES_SUPERVISOR);
}

bool PageTableEntry::get_accessed()
{
    return Bitmap::test(&entry, PGT_FLAGS_ACCESSED);
}

bool PageTableEntry::get_dirty()
{
    return Bitmap::test(&entry, PGT_FLAGS_DIRTY);
}

physical_addr PageTableEntry::get_frame()
{
    return (physical_addr)(entry & PGT_FRAME_MASK); // Make the lower 12 bits zero.
}

PageTableEntry PageTableEntry::from_bytes(uint32_t bytes)
{
    PageTableEntry e;
    e.entry = bytes;
    return e;
}