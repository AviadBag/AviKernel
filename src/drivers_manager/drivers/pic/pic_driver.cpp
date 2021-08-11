#include "drivers_manager/drivers/pic/pic_driver.h"
#include "utils/io.h"

#define MASTER 0
#define SLAVE 1

/* Ports */
#define MASTER_PIC_COMMAND_PORT 0x0020
#define MASTER_PIC_DATA_PORT 0x0021
#define SLAVE_PIC_COMMAND_PORT 0x00A0
#define SLAVE_PIC_DATA_PORT 0x00A1

#define MASTER_INTERRUPTS_OFFSET 0x20 // Where to remap to?
#define SLAVE_INTERRUPTS_OFFSET 0x28 // Where to remap to?

/* Commands */
#define END_OF_INTERRUPT 0x20

/* ICW's */
#define ICW1 0b00010001
#define ICW2_MASTER MASTER_INTERRUPTS_OFFSET
#define ICW2_SLAVE SLAVE_INTERRUPTS_OFFSET
#define ICW3_MASTER 0x04
#define ICW3_SLAVE 0x02
#define ICW4 0x01

void PICDriver::attach() 
{
    // ICW1
    send_command(MASTER, ICW1);
    IO::io_wait();
    send_command(SLAVE, ICW1);
    IO::io_wait();

    // ICW2
    send_data(MASTER, ICW2_MASTER);
    IO::io_wait();
    send_data(SLAVE, ICW2_SLAVE);
    IO::io_wait();

    // ICW3
    send_data(MASTER, ICW3_MASTER);
    IO::io_wait();
    send_data(SLAVE, ICW3_SLAVE);
    IO::io_wait();

    send_data(MASTER, ICW4);
    IO::io_wait();
    send_data(SLAVE, ICW4);
    IO::io_wait();
}

void PICDriver::detach() {} // Nothing here

void PICDriver::unmask_all_interrupts()
{
    send_data(MASTER, 0x0);
    send_data(SLAVE, 0x0);
}

// pic=0: master, pic=1: slave
void PICDriver::send_command(int pic, uint8_t command)
{
    if (pic > 1)
        return; // We have only two PIC's...

    uint16_t port = (pic == MASTER) ? MASTER_PIC_COMMAND_PORT : SLAVE_PIC_COMMAND_PORT;
    IO::outb(port, command);
}

// pic=0: master, pic=1: slave
uint8_t PICDriver::read_data(int pic)
{
    if (pic > 1)
        return 0; // We have only two PIC's...

    uint16_t port = (pic == MASTER) ? MASTER_PIC_DATA_PORT : SLAVE_PIC_DATA_PORT;
    return IO::inb(port);
}

// pic=0: master, pic=1: slave
void PICDriver::send_data(int pic, uint8_t data)
{
    if (pic > 1)
        return; // We have only two PIC's...

    uint16_t port = (pic == MASTER) ? MASTER_PIC_DATA_PORT : SLAVE_PIC_DATA_PORT;
    IO::outb(port, data);
}

void PICDriver::send_end_of_interrupt(uint8_t irq)
{
    if (irq >= 8)
        send_command(SLAVE, END_OF_INTERRUPT);

    send_command(MASTER, END_OF_INTERRUPT);
}

bool PICDriver::exist() 
{
    return true; // TODO: Only return true if there are TWO PIC's.
}
