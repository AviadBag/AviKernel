#ifndef _H_REGISTERS
#define _H_REGISTERS

#include <stdint.h>

class Registers
{
public:
    static uint32_t get_cr3();

    static void set_cr3(uint32_t);
};

#endif