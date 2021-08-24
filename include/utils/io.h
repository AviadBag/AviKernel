#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>

class IO {
public:
    static void outb(uint16_t port, uint8_t val);
    static void outw(uint16_t port, uint16_t val);
    static void outl(uint16_t port, uint32_t val);

    static uint8_t inb(uint16_t port);
    static uint16_t inw(uint16_t port);
    static uint32_t inl(uint16_t port);

    static void io_wait();
};
#endif // __IO_H__