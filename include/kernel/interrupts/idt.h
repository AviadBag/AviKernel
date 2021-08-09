#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define IDT_SIZE 256

struct idt_descriptor {
    uint16_t handler_lower;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t handler_higher;
} __attribute__((packed));

class IDT {
public:
    void install();

private:
    void put_descriptors();
    void put_descriptor(int index, uint32_t handler, uint16_t selector, uint8_t flags);

    idt_descriptor idt[IDT_SIZE];

    /* ------------------------- Singelton stuff. ------------------------- */
public:
    static IDT* get_instance();

    IDT(IDT& other) = delete; // Should not be cloneable.
    void operator=(const IDT& other) = delete;

private:
    IDT(); // This is a singelton - So a private constructor.

    static IDT* instance;
};
#endif