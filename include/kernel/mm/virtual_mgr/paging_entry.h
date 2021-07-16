#include <stdint.h>

#include "kernel/mm/physical_mgr/physical_mgr.h" // For physiacl_addr

#ifndef _PAGING_ENTRY_H
#define _PAGING_ENTRY_H

class PagingEntry {
public:
    PagingEntry(); // By default, all the values are zero.
    PagingEntry(bool present, bool writeable, bool requires_supervisor, physical_addr frame);

    uint32_t to_bytes();

    void set_frame(physical_addr addr); // Must be page aligned!
    void set_present(bool);
    void set_writeable(bool);
    void set_requires_supervisor(bool);

    bool get_present();
    bool get_writeable();
    bool get_requires_supervisor();
    bool get_accessed(); // Access flag
    physical_addr get_frame();

protected:
    uint32_t entry;
};

#endif