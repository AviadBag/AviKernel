#include "runtime/icxxabi.h"

#include "hal/drivers/keyboard/keyboard_driver.h"
#include "hal/drivers/clock/clock_driver.h"
#include "hal/drivers/pci/pci_driver.h"
#include "hal/drivers/pic/pic_driver.h"
#include "hal/hal.h"

#include "drivers/screen/text_output.h"
#include "drivers/screen/vga_text.h"
#include "drivers/serial_ports/serial_ports.h"

#include "kernel/gdt/gdt.h"
#include "kernel/interrupts/interrupts_manager.h"
#include "kernel/mm/heap/heap.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include "multiboot/multiboot.h"

#include "utils/queue.h"

#include <cstdio.h>
#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

void setup_memory_managment(multiboot_info_t* multiboot_info)
{
    GDT::initialize();
    PhysicalMgr::initialize(multiboot_info->mem_upper * 1024, multiboot_info->mmap_addr, multiboot_info->mmap_length);
    VirtualMgr::initialize();
    Heap::initialize();
}

void setup_drivers()
{
    HAL::get_instance()->initialize();

    PICDriver* pic_driver = (PICDriver*)HAL::get_instance()->get_driver(HAL_PIC_DRIVER);
    if (!pic_driver->exist())
        panic("This PC is unsupported, because it does not have 2 PIC's");
    pic_driver->attach();
    pic_driver->unmask_all_interrupts();
    asm volatile ("sti");

    PCIDriver* pci_driver = (PCIDriver*)HAL::get_instance()->get_driver(HAL_PCI_DRIVER);
    if (!pci_driver->exist())
        panic("This PC is unsupported, because it has no a PCI bus");
    pci_driver->attach();

    ClockDriver* clock_driver = (ClockDriver*)HAL::get_instance()->get_driver(HAL_CLOCK_DRIVER);
    if (!clock_driver->exist())
        panic("This PC is unsupported, because it has no clock");
    clock_driver->attach();

    KeyboardDriver* keyboard_driver = (KeyboardDriver*)HAL::get_instance()->get_driver(HAL_KEYBOARD_DRIVER);
    if (!keyboard_driver->exist()) 
        panic("No keyboard connected. Press F10 to continue.");
    keyboard_driver->attach();
}

extern "C" void kernel_main(multiboot_info_t* multiboot_info)
{
    // System Initialization
    VgaText::initialize();
    setup_memory_managment(multiboot_info);
    InterruptsManager::get_instance()->initialize();
    setup_drivers();

    // Endless loop!
    while (1) { }
}
