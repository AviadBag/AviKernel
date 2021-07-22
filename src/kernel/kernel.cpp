#include "runtime/icxxabi.h"

#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/text_input.h"
#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/screen/vga_text.h"
#include "drivers/serial_ports/serial_ports.h"
#include "drivers/pci/pci.h"

#include "kernel/gdt/gdt.h"
#include "kernel/idt/idt.h"
#include "kernel/mm/heap/heap.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include "multiboot/multiboot.h"

#include "utils/fixed_queue.h"
#include "utils/time.h"

#include <cstdio.h>
#include <cstdlib.h>
#include <stdint.h>

extern "C" void kernel_main(multiboot_info_t* multiboot_info)
{
    // Text
    VgaText::initialize();
    kprintf("Hello! Welcome to AviKernel!\n");

    // Memory
    GDT::initialize();
    PhysicalMgr::initialize(multiboot_info->mem_upper * 1024, multiboot_info->mmap_addr, multiboot_info->mmap_length);
    VirtualMgr::initialize();
    Heap::initialize();

    // Interrupts
    IDT::initialize();

    // Drivers
    PCI::initialize();
    PIC::initialize();
    PIC::enable_all_interrupts();
    asm volatile("sti");
    PIT::initialize(100); // Once every 0.01 second
    Keyboard::initialize();
    TextInput::initialize();

    // Utils
    Time::initialize();

    while (true) {
        char c = TextInput::getchar();
        kprintf("%c", c);
    }
}
