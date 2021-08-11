#include "runtime/icxxabi.h"

#include "drivers_manager/drivers/keyboard/keyboard_driver.h"
#include "drivers_manager/drivers/clock/clock_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pic/pic_driver.h"
#include "drivers_manager/drivers_manager.h"

#include "drivers/screen/text_output.h"
#include "drivers/screen/vga_text.h"
#include "utils/io.h"

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

#define MULTIBOOT_FLAGS_HAS_MEMORY_MAP 1 << 0

void setup_memory_managment(multiboot_info_t* multiboot_info)
{
    if (!(multiboot_info->flags & MULTIBOOT_FLAGS_HAS_MEMORY_MAP))
        panic("Bootloader didn't provide a memory map!.");
    
    GDT::initialize();
    PhysicalMgr::initialize(multiboot_info->mem_upper * 1024, multiboot_info->mmap_addr, multiboot_info->mmap_length);
    VirtualMgr::initialize();
    Heap::initialize();
}

void setup_drivers()
{
    DriversManager::get_instance()->initialize();

    PICDriver* pic_driver = (PICDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PIC_DRIVER);
    if (!pic_driver->exist())
        panic("This PC is unsupported, because it does not have 2 PIC's");
    pic_driver->attach();
    pic_driver->unmask_all_interrupts();
    asm volatile ("sti");

    PCIDriver* pci_driver = (PCIDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
    if (!pci_driver->exist())
        panic("This PC is unsupported, because it has no a PCI bus");
    pci_driver->attach();

    ClockDriver* clock_driver = (ClockDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_CLOCK_DRIVER);
    if (!clock_driver->exist())
        panic("This PC is unsupported, because it has no clock");
    clock_driver->attach();

    KeyboardDriver* keyboard_driver = (KeyboardDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_KEYBOARD_DRIVER);
    if (!keyboard_driver->exist()) 
        panic("No keyboard connected. Press F10 to continue.");
    keyboard_driver->attach();
}

void setup_interrupts()
{
    InterruptsManager::get_instance()->initialize();
}

// Registers default handlres to the CPU exceptions (Page Fault, Divide by Zero, etc...)
void setup_exceptions_handlers()
{
    InterruptsManager::get_instance()->set_isr(0, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Division by zero exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(1, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Debug exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(2, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Non maskable interrupt");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(3, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Breakpoint exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(4, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Into detected overflow");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(5, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Out of bounds exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(6, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Invalid opcode exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(7, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("No coprocessor exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(8, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Double fault");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(9, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Coprocessor segment overrun");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(10, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Bad TSS");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(11, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Segment not present");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(12, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Stack fault");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(13, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("General protection fault");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(14, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Page fault");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(15, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Unknown interrupt exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(16, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Coprocessor fault");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(17, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Alignment check exception");
    }, nullptr);
    InterruptsManager::get_instance()->set_isr(18, []([[gnu::unused]] void* context, [[gnu::unused]] uint32_t err) {
        panic("Machine check exception");
    }, nullptr);
}

extern "C" void kernel_main(multiboot_info_t* multiboot_info)
{
    // System Initialization
    VgaText::initialize();

    setup_memory_managment(multiboot_info);
    setup_interrupts();
    setup_exceptions_handlers();
    setup_drivers();

    // Endless loop!
    while (1) { }
}
