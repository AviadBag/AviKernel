#ifndef _SERIAL_PORTS_H
#define _SERIAL_PORTS_H

#include <stdint.h>

class SerialPorts {
public:
    static void outb(uint16_t port, uint8_t val);
    static void outl(uint16_t port, uint32_t val);

    static uint8_t  inb(uint16_t port);
    static uint32_t inl(uint16_t port);

    static void io_wait(void);
};

#endif