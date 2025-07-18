#include "runtime/icxxabi.h"

#include "drivers_manager/drivers/clock/clock_driver.h"
#include "drivers_manager/drivers/keyboard/keyboard_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pic/pic_driver.h"
#include "drivers_manager/drivers/storage/storage_driver.h"
#include "drivers_manager/drivers/video/video_driver.h"
#include "drivers_manager/drivers_manager.h"

#include "drivers/screen/text_output.h"
#include "drivers/screen/vga_text.h"

#include "kernel/gdt/gdt.h"
#include "kernel/interrupts/interrupts_manager.h"
#include "kernel/mm/heap/heap.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/panic.h"

#include "multiboot/multiboot.h"

#include "utils/io.h"
#include "utils/time.h"

#include "fs/devfs/devfs.h"
#include "fs/vfs/vfs.h"

#include "posix/errno.h"

#ifdef TEST
#include "test/tester.h"
#endif

#include <cstdio.h>
#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

#define MULTIBOOT_FLAGS_HAS_MEMORY_MAP 1 << 0

void setup_memory_managment(multiboot_info_t *multiboot_info)
{
    if (!(multiboot_info->flags & MULTIBOOT_FLAGS_HAS_MEMORY_MAP))
        panic("Bootloader didn't provide a memory map!.");

    GDT::initialize();
    PhysicalMgr::initialize(multiboot_info->mem_upper * 1024, multiboot_info->mmap_addr, multiboot_info->mmap_length);
    VirtualMgr::initialize();
    Heap::initialize();
}

void setup_drivers(multiboot_info_t *multiboot_info)
{
    DriversManager::get_instance()->initialize();

    PICDriver *pic_driver = (PICDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PIC_DRIVER);
    if (!pic_driver->exist())
        panic("This PC is unsupported, because it does not have 2 PIC's");
    pic_driver->setup_driver_and_device();
    pic_driver->unmask_all_interrupts();
    asm volatile("sti");

    PCIDriver *pci_driver = (PCIDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
    if (!pci_driver->exist())
        panic("This PC is unsupported, because it has no a PCI bus");
    pci_driver->setup_driver_and_device();
    printf("PCI devices:\n");
    for (int i = 0; i < pci_driver->get_devices()->size(); i++)
    {
        PCIDevice device = pci_driver->get_devices()->get(i);
        uint8_t bus_nubmer = device.get_bus_number();
        uint8_t device_number = device.get_device_number();
        uint8_t function_number = device.get_function_number();
        uint8_t class_code = pci_driver->get_class_code(device);
        uint8_t sub_class_code = pci_driver->get_sub_class_code(device);
        const char *desc = pci_driver->get_device_description(device);

        printf("Found PCI %d:%d:%d (0x%X:0x%X) -> %s\n", bus_nubmer, device_number, function_number, class_code, sub_class_code, desc);
    }

    ClockDriver *clock_driver = (ClockDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_CLOCK_DRIVER);
    if (!clock_driver->exist())
        panic("This PC is unsupported, because it has no clock");
    clock_driver->set_frequency(1000); // Once in MS.
    clock_driver->setup_driver_and_device();
    Time::initialize();

    StorageDriver *storage_driver = (StorageDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_STORAGE_DRIVER);
    printf("Storage Driver Exists? %s\n", (storage_driver->exist() ? "yes" : "no"));
    if (storage_driver->exist())
    {
        storage_driver->setup_driver_and_device();
        if (storage_driver->get_number_of_drives() > 0)
        {
            printf("------ Drives: ------\n");
            for (int i = 0; i < storage_driver->get_number_of_drives(); i++)
            {
                printf("Drive {%d} -> Drive Size: %lu MB\n", i, storage_driver->get_drive(i)->get_size_mb());
            }
        }
    }

    KeyboardDriver *keyboard_driver = (KeyboardDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_KEYBOARD_DRIVER);
    if (!keyboard_driver->exist())
        panic("No keyboard connected. Press F10 to continue.");
    keyboard_driver->setup_driver_and_device();

    VideoDriver *video_driver = (VideoDriver *)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_VIDEO_DRIVER);
    video_driver->set_multiboot_info(multiboot_info); // Must be called before anything
    if (!video_driver->exist())
        panic("No suitable video driver...");
    video_driver->setup_driver_and_device();

    for (uint32_t x = 0; x < video_driver->get_screen_width(); x++)
    {
        for (uint32_t y = 0; y < video_driver->get_screen_height(); y++)
        {
            video_driver->put_pixel(x, y, x % 256, y % 256, (x + y) % 256);
        }
    }
}

void setup_interrupts()
{
    InterruptsManager::get_instance()->initialize();
}

// Registers default handlres to the CPU exceptions (Page Fault, Divide by Zero, etc...)
void setup_exceptions_handlers()
{
    InterruptsManager::get_instance()->set_isr(
        0, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Division by zero exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        1, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Debug exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        2, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Non maskable interrupt"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        3, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Breakpoint exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        4, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Into detected overflow"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        5, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Out of bounds exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        6, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Invalid opcode exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        7, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("No coprocessor exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        8, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Double fault"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        9, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Coprocessor segment overrun"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        10, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Bad TSS"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        11, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Segment not present"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        12, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Stack fault"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        13, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("General protection fault"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        14, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Page fault"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        15, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Unknown interrupt exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        16, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Coprocessor fault"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        17, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Alignment check exception"); },
        nullptr);
    InterruptsManager::get_instance()->set_isr(
        18, []([[gnu::unused]] void *context, [[gnu::unused]] uint32_t err)
        { panic("Machine check exception"); },
        nullptr);
}

void DumpHex(const void *data, size_t size)
{
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            printf(" ");
            if ((i + 1) % 16 == 0)
            {
                printf("|  %s \n", ascii);
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

extern "C" void kernel_main(multiboot_info_t *multiboot_info)
{
    // System Initialization
    VgaText::initialize();

    setup_memory_managment(multiboot_info);
    setup_interrupts();
    setup_exceptions_handlers();
    setup_drivers(multiboot_info);

#ifdef TEST
    Tester::test();
#else
    FS *devfs = new DevFS();

    VFS vfs;
    if (!vfs.mount_fs("/dev/", "/", devfs))
        panic("Error mounting devfs");

    /* Clean up */
    devfs->umount();
    delete devfs;
#endif

    // Endless loop!
    while (1)
    {
    }
}
