#ifndef __PAGE_DIRECTORY_ENTRY_H__
#define __PAGE_DIRECTORY_ENTRY_H__

#include "kernel/mm/virtual_mgr/paging_entry.h"

class PageDirectoryEntry : public PagingEntry
{
public:
    PageDirectoryEntry();
    
    void set_4mb_page_size(bool);
    bool get_4mb_page_size();
};

#endif // __PAGE_DIRECTORY_ENTRY_H__