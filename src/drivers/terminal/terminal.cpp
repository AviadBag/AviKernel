#include "drivers/terminal/terminal.h"
#include <stdint.h>

#define ROWS 25
#define COLUMNS 80
#define MEMORY_ADDRESS 0xB8000
#define CHAR_ATTRIBUTES 0x07 // White on black

int Terminal::x;
int Terminal::y;

void Terminal::initialize()
{
    x = y = 0;
    clear();
}

void Terminal::putchar(char c)
{
    if (c == '\n')
    {
        next_line();
        if (y == ROWS)
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
}

void Terminal::backspace() 
{
    x--;
    if (x == -1)
    {
        if (y != 0) 
        {
            y--;
            x = COLUMNS-1;
        }
        else
            x = 0;
    }
    put_regular_char(' ', false);
}

void Terminal::put_regular_char(char c, bool next)
{
    if (y == ROWS)
        scroll();
    
    uint8_t *address = (uint8_t *)(MEMORY_ADDRESS + XYToOffset());
    *address = c;
    *(++address) = CHAR_ATTRIBUTES;

    if (next) next_char();
}

void Terminal::clear()
{
    for (int i = 0; i < ROWS * COLUMNS - 1; i++)
        put_regular_char(' ', true);
    put_regular_char(' ', false);
    x = 0;
    y = 0;
}

void Terminal::next_char()
{
    x++;
    if (x == COLUMNS)
        next_line();
}

void Terminal::next_line()
{
    x = 0;
    y++;
}

void Terminal::scroll()
{
    for (int row = 1; row < ROWS; row++) // Iterate evey row, from the second.
    {
        copy_row(row, row - 1);
    }

    x = 0;
    y--;
    for (int col = 0; col < COLUMNS - 1; col++) // Fill the last row with zeros
        put_regular_char(' ', true);
    put_regular_char(' ', false);
    x = 0; // Go to the start of the last row.
}

void Terminal::copy_row(int from, int to)
{
    int _x = x, _y = y; // Backup

    for (int i = 0; i < COLUMNS; i++) // Read from the source line, char by char.
    {
        y = from, x = i;
        uint8_t *address = (uint8_t *)(MEMORY_ADDRESS + XYToOffset());
        char c = *address;

        y = to, x = i;
        putchar(c); // Print it in the destination row.
    }

    x = _x, y = _y; // Restore
}

int Terminal::XYToOffset()
{
    return (x * 2) + (y * 2 * COLUMNS);
}