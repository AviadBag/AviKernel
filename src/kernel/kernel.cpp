#include "runtime/icxxabi.h"

#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/text_input.h"
#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/screen/vga_text.h"
#include "drivers/screen/text_output.h"
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
#include <cstring.h>

#define TERMINAL_COMMAND_MAX_SIZE 20

char* terminal_get_command()
{
    static char command[TERMINAL_COMMAND_MAX_SIZE+1];
    int number_of_chars = 0; // How many chars does the command contain?
    bool enter = false; // Was enter pressed?

    kprintf(">> ");
    while (!enter) // Get char
    {
        char c = TextInput::getchar();
        if (c == '\b') // Backspace
        {
            if (number_of_chars == 0)
                continue; // You cannot delete anymore
            else
            {
                number_of_chars--;
            }
        }
        else if (c == '\n') // Enter
            enter = true;
        else if (number_of_chars == TERMINAL_COMMAND_MAX_SIZE)
            continue; // You cannot type anymore
        else
            command[number_of_chars++] = c;

        kprintf("%c", c);
    }

    command[number_of_chars] = '\0'; // Null terminator

    return command;
}

void info()
{
    kprintf("AviKernel 0.1 - An OS developed by Aviad Bagno\n");
}

void type()
{
    kprintf("Please type here whatever you want, exit with '|'\n");
    int number_of_chars = 0; // To prevent previous lines override

    while (true)
    {
        char c = TextInput::getchar();

        if (c == '|') return;
        else if (c == '\b')
        {
            if (number_of_chars == 0)
                continue;
            else 
                number_of_chars -= 2; // Because it will be incremented by one soon.
        }

        number_of_chars++;
        kprintf("%c", c);
    }
}

void help()
{
    kprintf("Avialable commands:\n");
    kprintf("\"help\" - Shows this help message\n");
    kprintf("\"info\" - Shows information about this OS\n");
    kprintf("\"type\": A very simple text editor. Exit with '|'\n");
    kprintf("\"clear\": Clears the screen\n");
}

void command_not_found(const char* command)
{
    kprintf("\"%s\": Invalid command\n", command);
    help();
}

void terminal()
{
    help();
    while (true) // Get command
    {
        const char* command = terminal_get_command();
        if (strcmp(command, "info") == 0) info();
        else if (strcmp(command, "help") == 0) help();
        else if (strcmp(command, "type") == 0) type();
        else if (strcmp(command, "clear") == 0) TextOutput::clear();
        else if (*command != '\0') // Don't show "command not found" if the command was empty
            command_not_found(command);
    }
}

extern "C" void kernel_main(multiboot_info_t* multiboot_info)
{
    // Text
    VgaText::initialize();

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

    terminal();
}
