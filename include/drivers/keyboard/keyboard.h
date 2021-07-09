#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "extended_char.h" 

#include <stdint.h>

typedef void (*new_extended_char_listener)(ExtendedChar);

class Keyboard
{
public:
	static void initialize();
	static void set_new_extended_char_listener(new_extended_char_listener);

	static void on_ke_data(uint32_t unused); 
	static void on_make_scan_code(uint8_t scan_code); 

	static uint8_t ke_read_data(); // Reads a byte from the keyboard encoder

private:
	static new_extended_char_listener listener;
};

#endif
