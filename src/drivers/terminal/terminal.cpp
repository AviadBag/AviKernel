#include <drivers/terminal/terminal.h>
#include <cstdint.h>

#define ROWS 25
#define COLUMNS 80
#define MEMORY_ADDRESS 0xB8000
#define	CHAR_ATTRIBUTES 0x07 // White on black

Terminal::Terminal()
{
    x = y = 0;
}

void Terminal::print(char* str)
{
    while (*str) 
        print_char(*str++);
}

void Terminal::clear()
{
    for (int i = 0; i < ROWS * COLUMNS; i++) 
        print_char(' ');
    x = y = 0;   
}

void Terminal::next_char()
{
    x++;
    if (x == COLUMNS) next_line();
}

void Terminal::next_line()
{
    x = 0;
    y++;

    if (y == ROWS) scroll();
}

void Terminal::scroll()
{
    for (int row = 1; row < ROWS; row++) // Iterate evey row, from the second.
    {
        copy_row(row, row-1);
    }

    y--;
}

void Terminal::copy_row(int from, int to)
{
    int _x = x, _y = y; // Backup

    for (int i = 0; i < COLUMNS; i++) // Read from the source line, char by char.
    {
        y = from, x = i;
        uint8_t* address = (uint8_t*) (MEMORY_ADDRESS + XYToOffset());
        char c = *address;

        y = to, x = i;
        print_char(c); // Print it in the destination row.
    }

    x = _x, y = _y; // Restore
}

void Terminal::print_char(char c)
{
    switch (c)
    {
        case '\n':
            next_line();
            break;
        
        default:
            print_regular_char(c);
    }
}

void Terminal::print_regular_char(char c)
{
    uint8_t* address = (uint8_t*) (MEMORY_ADDRESS + XYToOffset());
    *address = c;
    *(++address) = CHAR_ATTRIBUTES;

    next_char();
}

int Terminal::XYToOffset()
{
    return (x * 2) + (y * 2 * COLUMNS);
}