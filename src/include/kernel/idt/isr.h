#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

struct registers
{
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, useless_value, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t interrupt_number, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, esp, ss; // Pushed by the processor automatically.
} __attribute__((packed));

extern "C" void isr_handler(struct registers regs);

#endif
