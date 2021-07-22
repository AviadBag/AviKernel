#include "drivers/screen/vga_text.h"

#include <stdint.h>
#include <cstdio.h>

#define VGA_ROWS 25
#define VGA_COLUMNS 80
#define VGA_MEMORY_ADDRESS 0xB8000
#define VGA_CHAR_ATTRIBUTES 0x07 // White on black

int VgaText::x;
int VgaText::y;

void VgaText::initialize()
{
    x = y = 0;
    clear();
    kprintf("Initialized VgaText...\n");
}

void VgaText::putchar(char c)
{
    if (c == '\n') {
        next_line();
        if (y == VGA_ROWS)
            scroll();
    } else if (c == '\t') {
        // A tab is four spaces.
        for (int i = 0; i < 4; i++)
            put_regular_char(' ', true);
    } else if (c == '\b') {
        backspace();
    } else
        put_regular_char(c, true);
}

void VgaText::backspace()
{
    x--;
    if (x == -1) {
        if (y != 0) {
            y--;
            x = VGA_COLUMNS - 1;
        } else
            x = 0;
    }
    put_regular_char(' ', false);
}

void VgaText::put_regular_char(char c, bool next)
{
    if (y == VGA_ROWS)
        scroll();

    uint8_t* address = (uint8_t*)(VGA_MEMORY_ADDRESS + XYToOffset());
    *address = c;
    *(++address) = VGA_CHAR_ATTRIBUTES;

    if (next)
        next_char();
}

void VgaText::clear()
{
    for (int i = 0; i < VGA_ROWS * VGA_COLUMNS - 1; i++)
        put_regular_char(' ', true);
    put_regular_char(' ', false);
    x = 0;
    y = 0;
}

void VgaText::next_char()
{
    x++;
    if (x == VGA_COLUMNS)
        next_line();
}

void VgaText::next_line()
{
    x = 0;
    y++;
}

void VgaText::scroll()
{
    for (int row = 1; row < VGA_ROWS; row++) // Iterate evey row, from the second.
    {
        copy_row(row, row - 1);
    }

    x = 0;
    y--;
    for (int col = 0; col < VGA_COLUMNS - 1; col++) // Fill the last row with zeros
        put_regular_char(' ', true);
    put_regular_char(' ', false);
    x = 0; // Go to the start of the last row.
}

void VgaText::copy_row(int from, int to)
{
    int _x = x, _y = y; // Backup

    for (int i = 0; i < VGA_COLUMNS; i++) // Read from the source line, char by char.
    {
        y = from, x = i;
        uint8_t* address = (uint8_t*)(VGA_MEMORY_ADDRESS + XYToOffset());
        char c = *address;

        y = to, x = i;
        putchar(c); // Print it in the destination row.
    }

    x = _x, y = _y; // Restore
}

int VgaText::XYToOffset()
{
    return (x * 2) + (y * 2 * VGA_COLUMNS);
}