#include <kernel/idt/idt.h>
#include <drivers/terminal/terminal.h>
#include <cstring.h>

extern "C" void install_idt(uint32_t, uint32_t);
extern "C" void first_handler();

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
    memset(&idt, 0, sizeof(idt_descriptor) * IDT_SIZE);

    set_descriptor(0, (uint32_t) isr0, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(1, (uint32_t) isr1, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(2, (uint32_t) isr2, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(3, (uint32_t) isr3, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(4, (uint32_t) isr4, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(5, (uint32_t) isr5, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(6, (uint32_t) isr6, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(7, (uint32_t) isr7, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(8, (uint32_t) isr8, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(9, (uint32_t) isr9, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(10, (uint32_t) isr10, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(11, (uint32_t) isr11, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(12, (uint32_t) isr12, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(13, (uint32_t) isr13, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(14, (uint32_t) isr14, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(15, (uint32_t) isr15, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(16, (uint32_t) isr16, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(17, (uint32_t) isr17, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(18, (uint32_t) isr18, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(19, (uint32_t) isr19, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(20, (uint32_t) isr20, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(21, (uint32_t) isr21, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(22, (uint32_t) isr22, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(23, (uint32_t) isr23, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(24, (uint32_t) isr24, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(25, (uint32_t) isr25, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(26, (uint32_t) isr26, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(27, (uint32_t) isr27, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(28, (uint32_t) isr28, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(29, (uint32_t) isr29, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(30, (uint32_t) isr30, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
    set_descriptor(31, (uint32_t) isr31, 0x8, INTERRUPT_IN_USE | PRIVILEGE_KERNEL);
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