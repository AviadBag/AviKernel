#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

class PIC
{
    public:
        static void remap_irq();
        static void send_end_of_interrupt(uint8_t irq);
        static void enable_all_interrupts();
    
    private:
        static void send_command(int pic, uint8_t command);
        static uint8_t read_data(int pic);
        static void send_data(int pic, uint8_t data);
};

#endif