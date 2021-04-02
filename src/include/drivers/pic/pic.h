#include <cstdint.h>

class PIC
{
    public:
        void remap_irq();
        void send_end_of_interrupt(uint8_t irq);
        void enable_all_interrupts();
    
    private:
        void send_command(int pic, uint8_t command);
        uint8_t read_data(int pic);
        uint8_t send_data(int pic, uint8_t data)
};