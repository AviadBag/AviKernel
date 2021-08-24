#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

class Time {
public:
    static void initialize();
    static void sleep(uint64_t ms);

private:
    static void on_tick();
    static uint64_t counter_ms;
};

#endif // __TIME_H__