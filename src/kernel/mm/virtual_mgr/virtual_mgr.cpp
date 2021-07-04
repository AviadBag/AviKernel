#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "utils/registers.h"

#include <cstring.h>

// Extracts the page table index from a virtual address
#define VMMGR_GET_PAGE_TABLE_INDEX(v_address) ((((uint32_t)(v_address)) >> 12) & 0x3FF)

// Extracts the page directory index from a virtual address
#define VMMGR_GET_PAGE_DIRECTORY_INDEX(v_address) ((((uint32_t)(v_address)) >> 22) & 0x3FF)

// Sets the table index in a given virtual address.
#define VMMGR_SET_PAGE_TABLE_INDEX(v_address, t_index) ((virtual_addr)((uint32_t)(v_address) | ((t_index) << 12)))

// Sets the directory index in a given virtual address.
#define VMMGR_SET_PAGE_DIRECTORY_INDEX(v_address, d_index) ((virtual_addr)((uint32_t)(v_address) | ((d_index) << 22)))

#define KERNEL_VIRTUAL_BASE_ADDR 0xC0000000

// Converts a kernel virtual address to it's physical form.
#define KERNEL_VIRTUAL_ADDR_TO_PHYSICAL(v_addr) ( (physical_addr) ((uint32_t)(v_addr) - KERNEL_VIRTUAL_BASE_ADDR))

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
    // TODO: TLB invalidation

    uint32_t index_in_page_directory = VMMGR_GET_PAGE_DIRECTORY_INDEX(v_addr);
    uint32_t index_in_page_table     = VMMGR_GET_PAGE_TABLE_INDEX(v_addr);

    PageDirectoryEntry page_directory_entry = PageDirectoryEntry::from_bytes(page_etable[index_in_page_directory]);
    virtual_addr page_table_virtual_addr = get_page_table_virtual_address(index_in_page_directory);
    uint32_t(*page_table)[1024] = (uint32_t(*)[1024])page_table_virtual_addr;

    // Check if the required page table already exists
    if (!page_directory_entry.get_present())
    {
        // The page table does not exist - create it
        physical_addr page_table_addr = PhysicalMgr::allocate_block();

        // Map a virtual address to the newly created page table
        PageTableEntry paeg_table_etable_entry(true, true, true, page_table_addr);
        page_etable[index_in_page_directory] = paeg_table_etable_entry.to_bytes();

        // Put the newly created page table in the page directory
        PageDirectoryEntry page_table_directory_entry(true, true, true, false, page_table_addr);
        // TODO: Put it in the page directory, and do invalidation.

        // Fil the newly created page table with zero's.
        memset(page_table_virtual_addr, 0, 4096);
    }

    // Map the desired page in the page table
    PageTableEntry page_table_entry(true, true, requires_supervisor, p_addr);
    (*page_table)[index_in_page_table] = page_table_entry.to_bytes();
}

void VirtualMgr::early_put_page_etable()
{
    PageDirectoryEntry page_etable_directory_entry(true, true, true, false, KERNEL_VIRTUAL_ADDR_TO_PHYSICAL(page_etable));

    // The page directory is stored in cr3
    uint32_t (*early_page_directory)[1024] = (uint32_t (*)[1024]) Registers::get_cr3();
    (*early_page_directory)[VMMGRE_PAGE_ETABLE_DIRECTORY_INDEX] = page_etable_directory_entry.to_bytes();

    // Reload CR3, to do TLB invalidation
    Registers::set_cr3(Registers::get_cr3());
}

void VirtualMgr::initialize()
{
    early_put_page_etable();
}