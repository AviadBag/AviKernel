#ifndef _IDT_H
#define _IDT_H

#include <cstdint.h>

#define IDT_SIZE 256

/* Idt flags */
#define INTERRUPT_IN_USE     0b10000000
#define INTERRUPT_NOT_IN_USE 0b00000000

#define PRIVILEGE_USER    0b01100000
#define PRIVILEGE_KERNEL  0b00000000

struct idt_descriptor
{
    uint16_t handler_lower;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t handler_higher;
} __attribute__((packed));

class IDT
{
    public:
        IDT();
        void install();

    private:
        void init_descriptors();
        void set_descriptor(int index, uint32_t handler, uint16_t selector, uint8_t flags);

        idt_descriptor idt[IDT_SIZE];
};

extern "C" void handler();

#endif