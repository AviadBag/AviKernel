#include <stdint.h>

#include "kernel/mm/physical_mgr/physical_mgr.h" // For physiacl_addr

#ifndef _PAGE_TABLE_ENTRY_H
#define _PAGE_TABLE_ENTRY_H

class PageTableEntry
{
public:
    PageTableEntry(); // By default, all the values are zero.
    uint32_t to_bytes();

    void set_frame(physical_addr addr); // Must be page aligned!
    void set_present(bool);
    void set_writeable(bool);
    void set_requires_supervisor(bool);

    bool get_present();
    bool get_writeable();
    bool get_requires_supervisor();
    bool get_accessed(); // Access flag
    bool get_dirty();  // Dirty flag
    physical_addr get_frame();

    static PageTableEntry from_bytes(uint32_t);

private:
    uint32_t entry;
};

#endif