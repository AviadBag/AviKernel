#ifndef __PAGE_DIRECTORY_ENTRY_H__
#define __PAGE_DIRECTORY_ENTRY_H__

#include "kernel/mm/virtual_mgr/paging_entry.h"

class PageDirectoryEntry : public PagingEntry
{
public:
    PageDirectoryEntry();
    
    PageDirectoryEntry(bool present, bool writeable, bool requires_supervisor, bool mb4_page_size, physical_addr frame);

    void set_4mb_page_size(bool);
    bool get_4mb_page_size();

    static PageDirectoryEntry from_bytes(uint32_t);
};

#endif // __PAGE_DIRECTORY_ENTRY_H__