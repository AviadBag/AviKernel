#include "kernel/mm/virtual_mgr/paging_entry.h"

#include "utils/bitmap.h"

enum PE_FLAGS_INDEXES {
    PE_FLAGS_PRESENT = 0,
    PE_FLAGS_WRITABLE = 1,
    PE_FLAGS_REQUIRES_SUPERVISOR = 2,
    PE_FLAGS_ACCESSED = 5,
};

#define PE_FRAME_MASK 0xFFFFF000

PagingEntry::PagingEntry()
    : entry(0)
{
}

PagingEntry::PagingEntry(bool present, bool writeable, bool requires_supervisor, physical_addr frame)
    : entry(0)
{
    set_present(present);
    set_writeable(writeable);
    set_requires_supervisor(requires_supervisor);
    set_frame(frame);
}

uint32_t PagingEntry::to_bytes()
{
    return entry;
}

void PagingEntry::set_frame(physical_addr addr)
{
    entry &= ~PE_FRAME_MASK; // Make the entry's address zero.
    uint32_t addr_p = (uint32_t)addr;
    addr_p &= PE_FRAME_MASK; // Make the address's lower bits zero.

    entry |= addr_p;
}

void PagingEntry::set_present(bool p)
{
    Bitmap::put(&entry, PE_FLAGS_PRESENT, p);
}

void PagingEntry::set_writeable(bool w)
{
    Bitmap::put(&entry, PE_FLAGS_WRITABLE, w);
}

void PagingEntry::set_requires_supervisor(bool r)
{
    Bitmap::put(&entry, PE_FLAGS_REQUIRES_SUPERVISOR, r);
}

bool PagingEntry::get_present()
{
    return Bitmap::test(&entry, PE_FLAGS_PRESENT);
}

bool PagingEntry::get_writeable()
{
    return Bitmap::test(&entry, PE_FLAGS_WRITABLE);
}

bool PagingEntry::get_requires_supervisor()
{
    return Bitmap::test(&entry, PE_FLAGS_REQUIRES_SUPERVISOR);
}

bool PagingEntry::get_accessed()
{
    return Bitmap::test(&entry, PE_FLAGS_ACCESSED);
}

physical_addr PagingEntry::get_frame()
{
    return (physical_addr)(entry & PE_FRAME_MASK); // Make the lower 12 bits zero.
}