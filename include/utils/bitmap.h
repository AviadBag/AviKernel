#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>

class Bitmap {
public:
    static void set(uint32_t* bitmap, int index);
    static void clear(uint32_t* bitmap, int index);
    static void put(uint32_t* bitmap, int index, bool what);
    static bool test(uint32_t* bitmap, int index);
};

#endif