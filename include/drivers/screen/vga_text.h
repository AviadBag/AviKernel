#ifndef __VGA_TEXT_H__
#define __VGA_TEXT_H__

#include <stdint.h>

#define VGA_ROWS 25
#define VGA_COLUMNS 80

class VgaText
{
public:
    static void putchar(char c);
    static void clear();
    static void initialize();

private:
    static void put_regular_char(char c, bool next); // Does not treat "\n" etc.
    static void next_char();
    static void next_line();
    static void backspace();
    static void scroll();
    static void copy_row(int from, int to);
    static uint16_t xy_to_memory_offset();
    static void move_cursor();
    static void initalize_cursor();
    static void write_data_register(uint8_t index, uint8_t what);

    static int x;
    static int y;
};

#endif // __VGA_TEXT_H__