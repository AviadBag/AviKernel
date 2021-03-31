#ifndef _TERMINAL_H
#define _TERMINAL_H

class Terminal
{
    public:
        void print(char* str);
        void clear();
        Terminal();

    private:
        void next_char();
        void next_line();
        void scroll();
        void copy_row(int from, int to);
        void print_char(char c);
        void print_regular_char(char c);
        int XYToOffset();

        int x;
        int y;
};

#endif