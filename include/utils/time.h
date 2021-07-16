#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

class Time {
public:
    static void initialize();
    static void sleep(uint64_t ms); // ms = milliseconds

private:
    static void on_tick(uint64_t unused);
    static uint64_t ticks;
};

#endif