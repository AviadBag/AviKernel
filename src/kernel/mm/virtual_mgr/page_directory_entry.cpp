#include "kernel/mm/virtual_mgr/page_directory_entry.h"

#include "utils/bitmap.h"

enum PDE_FLAGS_INDEXES {
    PDE_FLAGS_4MB = 7
};

PageDirectoryEntry::PageDirectoryEntry()
    : PagingEntry()
{
}

PageDirectoryEntry::PageDirectoryEntry(bool present, bool writeable, bool requires_supervisor, bool mb4_page_size, physical_addr frame)
    : PagingEntry(present, writeable, requires_supervisor, frame)
{
    set_4mb_page_size(mb4_page_size);
}

void PageDirectoryEntry::set_4mb_page_size(bool b)
{
    Bitmap::put(&entry, PDE_FLAGS_4MB, b);
}

bool PageDirectoryEntry::get_4mb_page_size()
{
    return Bitmap::test(&entry, PDE_FLAGS_4MB);
}

PageDirectoryEntry PageDirectoryEntry::from_bytes(uint32_t bytes)
{
    PageDirectoryEntry e;
    e.entry = bytes;
    return e;
}