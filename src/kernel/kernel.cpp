#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/terminal/terminal.h"

#include "kernel/gdt/gdt.h"
#include "kernel/idt/idt.h"

#include "multiboot/multiboot.h"

#include <cstdio.h>
#include <cstdlib.h>

void on_tick(uint32_t unused)
{
    static int counter = 0;
    counter++;
    if (counter % 100 == 0) // This is a second
    {
        int seconds = counter / 100;
        //printf("Second: %d\n", seconds);
    }
}

extern "C" void kernel_main(multiboot_info_t* multiboot_info)
{
    Terminal::initialize();
    printf("Hello! Welcome to AviKernel!\n");

    multiboot_memory_map_t* entry = (multiboot_memory_map_t*) multiboot_info->mmap_addr;
    while ((uint32_t) entry < (multiboot_info->mmap_addr + multiboot_info->mmap_length))
    {
        //printf("Base: %lx, Length: %lx, Type: %x, Size: %d\n", entry->addr, entry->len, entry->type, entry->size);
        printf("Base: %lx", entry->addr);
        printf(", Length: %lx", entry->len);
        printf(", Type: %lx", entry->type);
        printf(", Size: %d\n", entry->size);

        // Go to next entry
        entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size)); 
    }

    GDT::initialize();
    PIC::initialize();
    PIC::enable_all_interrupts();
    IDT::initialize();
    asm("sti");
    PIT::initialize(100, on_tick); // Once every 0.01 second

    while (1) { }
}