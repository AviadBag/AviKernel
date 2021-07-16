#include "utils/registers.h"

uint32_t Registers::get_cr3()
{
    uint32_t cr3 = 0;
    asm volatile("mov %%cr3, %0"
                 : "=r"(cr3));
    return cr3;
}

void Registers::set_cr3(uint32_t cr3)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(cr3));
}