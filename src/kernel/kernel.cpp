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

#define TERMINAL_COMMAND_MAX_SIZE 20

Queue<char> chars_queue;

void new_char_listener(ExtendedChar c)
{
    if (c.printable())
        chars_queue.enqueue(c.as_regular_char());
}

char getchar()
{
    while (chars_queue.empty())
        ;
    
    return chars_queue.dequeue();
}

char* terminal_get_command()
{
    static char command[TERMINAL_COMMAND_MAX_SIZE+1];
    int number_of_chars = 0; // How many chars does the command contain?
    bool enter = false; // Was enter pressed?

    kprintf(">> ");
    while (!enter) // Get char
    {
        char c = getchar();
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
    VgaText::clear();
    kprintf("Please type here whatever you want, exit with '|'\n");
    int number_of_chars = 0; // To prevent previous lines override

    while (true)
    {
        char c = getchar();

        if (c == '|')
        {
            VgaText::clear();
            return;
        }
        else if (c == '\b')
        {
            if (number_of_chars == 0)
                continue;
            else 
                number_of_chars -= 2; // Because it will be incremented by one soon.
        }
        else if (c == '\n')
            number_of_chars += VGA_COLUMNS;

        number_of_chars++;
        kprintf("%c", c);
    }
}

void help()
{
    kprintf("Avialable commands:\n");
    kprintf("\"help\" - Shows this help message\n");
    kprintf("\"info\" - Shows information about this OS\n");
    kprintf("\"type\" - A very simple text editor. Exit with '|'\n");
    kprintf("\"clear\" - Clears the screen\n");
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

const char* pci_device_type_to_string(uint8_t class_code, uint8_t sub_class_code) // Taken from https://github.com/MandelbrotOS/MandelbrotOS/blob/master/src/pci/pci_descriptors.c
{
    switch (class_code) {
    case 0:
        return "Unknown";
    case 1: {
        switch (sub_class_code) {
        case 0:
            return "SCSI storage controller";
        case 1:
            return "IDE interface";
        case 2:
            return "Floppy disk controller";
        case 3:
            return "IPI bus controller";
        case 4:
            return "RAID bus controller";
        case 5:
            return "ATA controller";
        case 6:
            return "SATA controller";
        case 7:
            return "Serial Attached SCSI controller";
        case 8:
            return "Non-Volatile memory controller";
        default:
            return "Mass Storage Controller";
        }
    }

    case 2: {
        switch (sub_class_code) {
        case 0:
            return "Ethernet controller";
        case 1:
            return "Token ring network controller";
        case 2:
            return "FDDI network controller";
        case 3:
            return "ATM network controller";
        case 4:
            return "ISDN controller";
        case 5:
            return "WorldFip controller";
        case 6:
            return "PICMG controller";
        case 7:
            return "Infiniband controller";
        case 8:
            return "Fabric controller";
        default:
            return "Network controller";
        }
    }

    case 3:
        switch (sub_class_code) {
        case 0:
            return "VGA compatible controller";
        case 1:
            return "XGA compatible controller";
        case 2:
            return "3D controller";
        default:
            return "Display controller";
        }
    case 4:
        switch (sub_class_code) {
        case 0:
            return "Multimedia video controller";
        case 1:
            return "Multimedia audio controller";
        case 2:
            return "Computer telephony device";
        case 3:
            return "Audio device";
        default:
            return "Multimedia controller";
        }
    case 5:
        switch (sub_class_code) {
        case 0:
            return "RAM memory";
        case 1:
            return "FLASH memory";
        default:
            return "Memory controller";
        }
    case 6:
        switch (sub_class_code) {
        case 0:
            return "Host bridge";
        case 1:
            return "ISA bridge";
        case 2:
            return "EISA bridge";
        case 3:
            return "MicroChannel bridge";
        case 4:
            return "PCI bridge";
        case 5:
            return "PCMCIA bridge";
        case 6:
            return "NuBus bridge";
        case 7:
            return "CardBus bridge";
        case 8:
            return "RACEway bridge";
        case 9:
            return "Semi-transparent PCI-to-PCI bridge";
        case 0xa:
            return "InfiniBand to PCI host bridge";
        default:
            return "Bridge";
        }
    case 7:
        switch (sub_class_code) {
        case 0:
            return "Serial controller";
        case 1:
            return "Parallel controller";
        case 2:
            return "Multiport serial controller";
        case 3:
            return "Modem";
        case 4:
            return "GPIB controller";
        case 5:
            return "Smard Card controller";
        default:
            return "Communication controller";
        }
    case 8:
        switch (sub_class_code) {
        case 0:
            return "PIC";
        case 1:
            return "DMA controller";
        case 2:
            return "Timer";
        case 3:
            return "RTC";
        case 4:
            return "PCI Hot-plug controller";
        case 5:
            return "SD Host controller";
        case 6:
            return "IOMMU";
        default:
            return "System peripheral";
        }
    case 9:
        switch (sub_class_code) {
        case 0:
            return "Keyboard controller";
        case 1:
            return "Digitizer Pen";
        case 2:
            return "Mouse controller";
        case 3:
            return "Scanner controller";
        case 4:
            return "Gameport controller";
        default:
            return "Input device controller";
        }
    case 0xa:
        switch (sub_class_code) {
        case 0:
            return "Generic Docking Station";
        default:
            return "Docking Station";
        }
    case 0xb:
        switch (sub_class_code) {
        case 0:
            return "386";
        case 1:
            return "486";
        case 2:
            return "Pentium";
        case 10:
            return "Alpha";
        case 20:
            return "Power PC";
        case 30:
            return "MIPS";
        case 40:
            return "Co-processor";
        default:
            return "Processor";
        }
    case 0xc:
        switch (sub_class_code) {
        case 0:
            return "FireWire (IEEE 1394)";
        case 1:
            return "ACCESS Bus";
        case 2:
            return "SSA";
        case 3:
            return "USB controller";
        case 4:
            return "Fibre Channel";
        case 5:
            return "SMBus";
        case 6:
            return "InfiniBand";
        case 7:
            return "IPMI Interface";
        case 8:
            return "SERCOS interface";
        case 9:
            return "CANBUS";
        default:
            return "Serial bus controller";
        }
    case 0xd:
        switch (sub_class_code) {
        case 0:
            return "IRDA controller";
        case 1:
            return "Consumer IR controller";
        case 10:
            return "RF controller";
        case 11:
            return "Bluetooth";
        case 12:
            return "Broadband";
        case 20:
            return "802.1a controller";
        case 21:
            return "802.1b controller";
        default:
            return "Wireless controller";
        }
    case 0xe:
        switch (sub_class_code) {
        case 0:
            return "I2O";
        default:
            return "Intelligent controller";
        }
    default:
        return "Unknown device";
    }
}

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
    kprintf("\nPCI Devices:\n");
    for (int i = 0; i < pci_driver->get_devices()->size(); i++)
    {
        PCIDevice d = pci_driver->get_devices()->get(i);
        int class_code = pci_driver->get_class_code(d);
        int sub_class_code = pci_driver->get_sub_class_code(d);
        kprintf("  Name: %s, PROG IF: 0x%X\n", pci_device_type_to_string(class_code, sub_class_code), pci_driver->get_prog_if(d));
    }
    kprintf("\n");

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

    // Terminal Setup
    KeyboardDriver* keyboard_driver = (KeyboardDriver*)HAL::get_instance()->get_driver(HAL_KEYBOARD_DRIVER);
    keyboard_driver->set_on_press_listener(new_char_listener);
    terminal();

    // Endless loop!
    while (1) { }
}
