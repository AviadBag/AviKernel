#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/extended_char.h"
#include "drivers/serial_ports/serial_ports.h"
#include "kernel/idt/isr.h"

#include <cstdio.h>

#define KEYBOARD_INTERRUPT_NUBMER 33 

#define KE_PORT 0x60
#define KC_PORT 0x64

#define MAKE_SCAN_CODES_START 0x1
#define MAKE_SCAN_CODES_END   0x58

#define BREAK_SCAN_CODE_START (MAKE_SCAN_CODES_START | 0x80)
#define BREAK_SCAN_CODE_END (MAKE_SCAN_CODES_END | 0x80)

new_extended_char_listener Keyboard::press_listener, Keyboard::release_listener;

const char* avi_charcode_to_string(avi_charcode c);

void Keyboard::initialize()
{
	register_isr(KEYBOARD_INTERRUPT_NUBMER, Keyboard::on_ke_data);
}

void Keyboard::set_press_extended_char_listener(new_extended_char_listener l)
{
	press_listener = l;	
}

void Keyboard::set_release_extended_char_listener(new_extended_char_listener l)
{
	release_listener = l;	
}

void log(const char* s)
{
	while (*s)
	{
		SerialPorts::outb(0x3F8, *s);
		s++;
	}
}

void Keyboard::on_ke_data(uint32_t unused)
{
	uint8_t data = ke_read_data();
	bool is_break = false;
	
	// Is it a break scan code?
	if (data >= BREAK_SCAN_CODE_START && data <= BREAK_SCAN_CODE_END)
	{
		is_break = true;
		data &= ~0x80; // Convert it to a regular scan code. 
	}

	// Is it a scan code?
	if (data >= MAKE_SCAN_CODES_START && data <= MAKE_SCAN_CODES_END)
	{
		if (is_break)
			on_break_scan_code(data);
		else
			on_make_scan_code(data);
	}
}

void Keyboard::on_make_scan_code(uint8_t scan_code)
{
	ExtendedChar ec(scan_code);
	if (press_listener) press_listener(ec);
}

void Keyboard::on_break_scan_code(uint8_t scan_code)
{
	ExtendedChar ec(scan_code);
	if (release_listener) release_listener(ec);
}

uint8_t Keyboard::ke_read_data()
{
	return SerialPorts::inb(KE_PORT);
}
