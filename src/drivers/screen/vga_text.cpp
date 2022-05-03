#include "drivers/screen/vga_text.h"
#include "utils/io.h"

#include <cstdio.h>
#include <stdint.h>

#define VGA_MEMORY_ADDRESS 0xB8000
#define VGA_CHAR_ATTRIBUTES 0x17 // White on blue

#define VGA_INDEX_REGISTER 0x3d4
#define VGA_DATA_REGISTER 0x3d5
#define VGA_CURSOR_START_INDEX 10
#define VGA_CURSOR_END_INDEX 11
#define VGA_CURSOR_OFFSET_HIGH_INDEX 14
#define VGA_CURSOR_OFFSET_LOW_INDEX 15

int VgaText::x;
int VgaText::y;

void VgaText::initialize()
{
    x = y = 0;
    clear();
    initalize_cursor();
    printf("Initialized VgaText...\n");
}

void VgaText::initalize_cursor()
{
    write_data_register(VGA_CURSOR_START_INDEX, 0); // No blinking
    move_cursor();                                  // Put the cursor in 0,0
}

void VgaText::putchar(char c)
{
    if (c == '\n')
    {
        next_line();
        if (y == VGA_ROWS)
            scroll();
    }
    else if (c == '\t')
    {
        // A tab is four spaces.
        for (int i = 0; i < 4; i++)
            put_regular_char(' ', true);
    }
    else if (c == '\b')
    {
        backspace();
    }
    else
        put_regular_char(c, true);

    move_cursor();
}

void VgaText::move_cursor()
{
    uint16_t offset = y * VGA_COLUMNS + x;
    uint8_t offset_low = (uint8_t)offset;
    uint8_t offset_high = (uint8_t)(offset >> 8);

    write_data_register(VGA_CURSOR_OFFSET_HIGH_INDEX, offset_high);
    write_data_register(VGA_CURSOR_OFFSET_LOW_INDEX, offset_low);
}

void VgaText::write_data_register(uint8_t index, uint8_t what)
{
    // Give the index
    IO::outb(VGA_INDEX_REGISTER, index);

    // Give the data
    IO::outb(VGA_DATA_REGISTER, what);
}

void VgaText::backspace()
{
    x--;
    if (x == -1)
    {
        if (y != 0)
        {
            y--;
            x = VGA_COLUMNS - 1;
        }
        else
            x = 0;
    }
    put_regular_char(' ', false);
}

void VgaText::put_regular_char(char c, bool next)
{
    if (y == VGA_ROWS)
        scroll();

    uint8_t *address = (uint8_t *)(VGA_MEMORY_ADDRESS + xy_to_memory_offset());
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
        uint8_t *address = (uint8_t *)(VGA_MEMORY_ADDRESS + xy_to_memory_offset());
        char c = *address;

        y = to, x = i;
        putchar(c); // Print it in the destination row.
    }

    x = _x, y = _y; // Restore
}

uint16_t VgaText::xy_to_memory_offset()
{
    return (x * 2) + (y * 2 * VGA_COLUMNS);
}