#ifndef _TERMINAL_H
#define _TERMINAL_H

class Terminal {
public:
    static void putchar(char c);
    static void clear();
    static void initialize();

private:
    static void put_regular_char(char c, bool next); // Does not treat "\n" etc.
    static void next_char();
    static void next_line();
    static void scroll();
    static void copy_row(int from, int to);
    static int XYToOffset();

    static int x;
    static int y;
};

#endif