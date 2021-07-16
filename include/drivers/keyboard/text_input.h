#ifndef _TEXT_INPUT_H
#define _TEXT_INPUT_H

#include "extended_char.h"
#include "utils/fixed_queue.h"

class TextInput {
public:
    static void initialize();
    static char getchar();
    static void on_press_extended_char(ExtendedChar);
    static void on_release_extended_char(ExtendedChar);

private:
    static char apply_shift(char);
    static char apply_caps_lock(char);

    static FixedQueue<char> queue;
    static bool shift, caps_lock;
};

#endif
