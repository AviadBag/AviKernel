#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"

#include <cstring.h>

// Extracts the page table index from a virtual address
#define VMMGR_GET_PAGE_TABLE_INDEX(v_address) ((((uint32_t)(v_address)) >> 12) & 0x3FF)

// Extracts the page directory index from a virtual address
#define VMMGR_GET_PAGE_DIRECTORY_INDEX(v_address) ((((uint32_t)(v_address)) >> 22) & 0x3FF)

// Sets the table index in a given virtual address.
#define VMMGR_SET_PAGE_TABLE_INDEX(v_address, t_index) ((virtual_addr)((uint32_t)(v_address) | ((t_index) << 12)))

// Sets the directory index in a given virtual address.
#define VMMGR_SET_PAGE_DIRECTORY_INDEX(v_address, d_index) ((virtual_addr)((uint32_t)(v_address) | ((d_index) << 22)))

// The page etable is the last entry in the page directory.
#define VMMGRE_PAGE_ETABLE_DIRECTORY_INDEX 1023

uint32_t VirtualMgr::page_directory[1024];
uint32_t VirtualMgr::page_etable[1024];

virtual_addr VirtualMgr::get_page_table_virtual_address(int index)
{
    virtual_addr addr = 0;
    addr = VMMGR_SET_PAGE_TABLE_INDEX(addr, index);
    addr = VMMGR_SET_PAGE_DIRECTORY_INDEX(addr, VMMGRE_PAGE_ETABLE_DIRECTORY_INDEX);
    return addr;
}

void VirtualMgr::map(virtual_addr v_addr, physical_addr p_addr, bool requires_supervisor)
{
    uint32_t page_table_index = VMMGR_GET_PAGE_TABLE_INDEX(v_addr);

    PageDirectoryEntry page_directory_entry = PageDirectoryEntry::from_bytes(page_etable[page_table_index]);
    virtual_addr page_table_virtual_addr = get_page_table_virtual_address(page_table_index);
    uint32_t(*page_table)[1024] = (uint32_t(*)[1024])page_table_virtual_addr;

    // Check if the required page table already exists
    if (!page_directory_entry.get_present())
    {
        // The page table does not exist - create it
        physical_addr page_table_addr = PhysicalMgr::allocate_block();

        // Map a virtual address to the newly created page table
        PageTableEntry page_etable_entry;
        page_etable_entry.set_present(true);
        page_etable_entry.set_requires_supervisor(true);
        page_etable_entry.set_writeable(true);
        page_etable_entry.set_frame(page_table_addr);
        page_etable[page_table_index] = page_etable_entry.to_bytes();

        // Put the newly created page table in the page directory
        PageDirectoryEntry page_directory_entry;
        page_directory_entry.set_4mb_page_size(false);
        page_directory_entry.set_present(true);
        page_directory_entry.set_requires_supervisor(true);
        page_directory_entry.set_writeable(true);
        page_directory_entry.set_frame(page_table_addr);

        // Fil the newly created page table with zero's.
        memset(page_table_virtual_addr, 0, 4096);
    }

    // Map the desired page in the page table
    PageTableEntry page_table_entry;
    page_table_entry.set_present(true);
    page_table_entry.set_requires_supervisor(requires_supervisor);
    page_table_entry.set_writeable(true);
    page_table_entry.set_frame(v_addr);
    (*page_table)[page_table_index] = page_table_entry.to_bytes();
}

void VirtualMgr::initialize()
{
    
}