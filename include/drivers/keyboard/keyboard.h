#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "extended_char.h"

#include <stdint.h>

typedef void (*new_extended_char_listener)(ExtendedChar);

class Keyboard {
public:
    static void initialize();
    static void set_press_extended_char_listener(new_extended_char_listener);
    static void set_release_extended_char_listener(new_extended_char_listener);

    static void on_ke_data(uint32_t unused);
    static void on_make_scan_code(uint8_t scan_code);
    static void on_break_scan_code(uint8_t scan_code);
    static bool ignore(uint8_t scan_code); /* Shall I ignore this scan code? */

    static uint8_t ke_read_data(); // Reads a byte from the keyboard encoder

private:
    static new_extended_char_listener press_listener, release_listener;
    static bool wait_for_second_scan_code; /* Shall I wait for a second scan code? */
    static bool wait_for_second_and_third_scan_code; /* Shall I wait for a second and a third scan code? */
};

#endif
