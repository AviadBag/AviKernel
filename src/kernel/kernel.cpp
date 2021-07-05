#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/terminal/terminal.h"

#include "kernel/gdt/gdt.h"
#include "kernel/idt/idt.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include "multiboot/multiboot.h"

#include "utils/time.h"

#include <cstdio.h>
#include <cstdlib.h>
#include <stdint.h>

extern "C" void kernel_main(multiboot_info_t *multiboot_info)
{
	Terminal::initialize();
	kprintf("Hello! Welcome to AviKernel!\n");

	GDT::initialize();
	PIC::initialize();
	PIC::enable_all_interrupts();
	IDT::initialize();
	asm volatile ("sti");
	PIT::initialize(100); // Once every 0.01 second
	PhysicalMgr::initialize(multiboot_info->mem_upper * 1024, multiboot_info->mmap_addr, multiboot_info->mmap_length);
	Time::initialize();
	VirtualMgr::initialize();

	while (true)
	{
	}
}