#ifndef _PIC_DRIVER_H__
#define _PIC_DRIVER_H__

#include "drivers_manager/drivers/driver.h"

#include <stdint.h>

class PICDriver : public Driver
{
public:
    virtual void setup_driver_and_device();
    virtual void detach();
    virtual bool exist(); // Returns false if there are no two PIC's.

    void send_end_of_interrupt(uint8_t irq);
    void unmask_all_interrupts();

private:
    void send_command(int pic, uint8_t command);
    uint8_t read_data(int pic);
    void send_data(int pic, uint8_t data);
};

#endif // _PIC_DRIVER_H__