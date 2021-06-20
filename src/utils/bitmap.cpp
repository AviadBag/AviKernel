#include "utils/bitmap.h"

#define BITMAP_BITS_PER_CELL 32

#define INDEX_TO_OFFSET(index) ((index) / BITMAP_BITS_PER_CELL) // Index to offset in the array
#define INDEX_TO_BIT(index) ((index) % BITMAP_BITS_PER_CELL) // Index to bit number in a cell

void Bitmap::set(uint32_t* bitmap, int index)
{
    bitmap[INDEX_TO_OFFSET(index)] |= (1 << INDEX_TO_BIT(index));
}

void Bitmap::clear(uint32_t* bitmap, int index)
{
    bitmap[INDEX_TO_OFFSET(index)] &= ~(1 << INDEX_TO_BIT(index));
}

bool Bitmap::test(uint32_t* bitmap, int index)
{
    int result = bitmap[INDEX_TO_OFFSET(index)] & (1 << INDEX_TO_BIT(index));
    return result > 0;
}
