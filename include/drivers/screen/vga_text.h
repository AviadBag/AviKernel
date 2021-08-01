#ifndef __VGA_TEXT_H__
#define __VGA_TEXT_H__

#define VGA_ROWS 25
#define VGA_COLUMNS 80

class VgaText {
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
    static int XYToOffset();

    static int x;
    static int y;
};

#endif // __VGA_TEXT_H__