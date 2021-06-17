#ifndef _TERMINAL_H
#define _TERMINAL_H

class Terminal
{
    public:
        void putchar(char c);
        void clear();
        Terminal();

    private:
        void put_regular_char(char c); // Does not treat "\n" etc.
        void next_char();
        void next_line();
        void scroll();
        void copy_row(int from, int to);
        int XYToOffset();

        int x;
        int y;
};

#endif