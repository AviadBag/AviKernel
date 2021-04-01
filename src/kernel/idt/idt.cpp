#include <kernel/idt/idt.h>
#include <drivers/terminal/terminal.h>
#include <cstdlib.h>

extern Terminal terminal;
extern "C" void install_idt(uint32_t, uint32_t);

IDT::IDT()
{
    init_descriptors();
}

void IDT::install()
{
    uint32_t size = sizeof(idt_descriptor) * IDT_SIZE - 1;
    install_idt((uint32_t) &idt, size); // Calls the assembly function
}

void IDT::init_descriptors()
{
    for (int i = 0; i < IDT_SIZE; i++)
    {
        set_descriptor(i, (uint32_t) &default_handler, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    }
}

extern "C" void default_handler()
{
    
}

void IDT::set_descriptor(int index, uint32_t handler, uint16_t selector, uint8_t flags)
{
    idt_descriptor descriptor;

    descriptor.handler_lower = handler & 0xFFFF; // Extract lower 16 bits
    descriptor.handler_higher = (handler >> 16) & 0xFFFF; // Extract heigher 16 bits

    descriptor.selector = selector;
    descriptor.zero = 0;
    descriptor.flags = flags | 0b00001110;

    idt[index] = descriptor;
}