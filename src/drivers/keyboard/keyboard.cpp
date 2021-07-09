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

new_extended_char_listener Keyboard::listener;

const char* avi_charcode_to_string(avi_charcode c);

void Keyboard::initialize()
{
	register_isr(KEYBOARD_INTERRUPT_NUBMER, Keyboard::on_ke_data);
}

void Keyboard::set_new_extended_char_listener(new_extended_char_listener l)
{
	listener = l;	
}

void Keyboard::on_ke_data(uint32_t unused)
{
	uint8_t data = ke_read_data();

	if (data >= MAKE_SCAN_CODES_START && data <= MAKE_SCAN_CODES_END)
	{
		on_make_scan_code(data);
	}
}

void Keyboard::on_make_scan_code(uint8_t scan_code)
{
	ExtendedChar ec(scan_code);
	listener(ec);
}

uint8_t Keyboard::ke_read_data()
{
	return SerialPorts::inb(KE_PORT);
}

const char* avi_charcode_to_string(avi_charcode c)
{
	if (c == KEY_UNKNOWN)
		return "KEY_UNKNOWN";
	if (c == KEY_ESCAPE)
		return "KEY_ESCAPE";
	if (c == KEY_1)
		return "KEY_1";
	if (c == KEY_2)
		return "KEY_2";
	if (c == KEY_3)
		return "KEY_3";
	if (c == KEY_4)
		return "KEY_4";
	if (c == KEY_5)
		return "KEY_5";
	if (c == KEY_6)
		return "KEY_6";
	if (c == KEY_7)
		return "KEY_7";
	if (c == KEY_8)
		return "KEY_8";
	if (c == KEY_9)
		return "KEY_9";
	if (c == KEY_0)
		return "KEY_0";
	if (c == KEY_MINUS)
		return "KEY_MINUS";
	if (c == KEY_EQUAL)
		return "KEY_EQUAL";
	if (c == KEY_BACKSPACE)
		return "KEY_BACKSPACE";
	if (c == KEY_TAB)
		return "KEY_TAB";
	if (c == KEY_Q)
		return "KEY_Q";
	if (c == KEY_W)
		return "KEY_W";
	if (c == KEY_E)
		return "KEY_E";
	if (c == KEY_R)
		return "KEY_R";
	if (c == KEY_T)
		return "KEY_T";
	if (c == KEY_Y)
		return "KEY_Y";
	if (c == KEY_U)
		return "KEY_U";
	if (c == KEY_I)
		return "KEY_I";
	if (c == KEY_O)
		return "KEY_O";
	if (c == KEY_P)
		return "KEY_P";
	if (c == KEY_LEFTBRACKET)
		return "KEY_LEFTBRACKET";
	if (c == KEY_RIGHTBRACKET)
		return "KEY_RIGHTBRACKET";
	if (c == KEY_RETURN)
		return "KEY_RETURN";
	if (c == KEY_LCTRL)
		return "KEY_LCTRL";
	if (c == KEY_A)
		return "KEY_A";
	if (c == KEY_S)
		return "KEY_S";
	if (c == KEY_D)
		return "KEY_D";
	if (c == KEY_F)
		return "KEY_F";
	if (c == KEY_G)
		return "KEY_G";
	if (c == KEY_H)
		return "KEY_H";
	if (c == KEY_J)
		return "KEY_J";
	if (c == KEY_K)
		return "KEY_K";
	if (c == KEY_L)
		return "KEY_L";
	if (c == KEY_SEMICOLON)
		return "KEY_SEMICOLON";
	if (c == KEY_QUOTE)
		return "KEY_QUOTE";
	if (c == KEY_GRAVE)
		return "KEY_GRAVE";
	if (c == KEY_LSHIFT)
		return "KEY_LSHIFT";
	if (c == KEY_BACKSLASH)
		return "KEY_BACKSLASH";
	if (c == KEY_Z)
		return "KEY_Z";
	if (c == KEY_X)
		return "KEY_X";
	if (c == KEY_C)
		return "KEY_C";
	if (c == KEY_V)
		return "KEY_V";
	if (c == KEY_B)
		return "KEY_B";
	if (c == KEY_N)
		return "KEY_N";
	if (c == KEY_M)
		return "KEY_M";
	if (c == KEY_COMMA)
		return "KEY_COMMA";
	if (c == KEY_DOT)
		return "KEY_DOT";
	if (c == KEY_SLASH)
		return "KEY_SLASH";
	if (c == KEY_RSHIFT)
		return "KEY_RSHIFT";
	if (c == KEY_KP_ASTERISK)
		return "KEY_KP_ASTERISK";
	if (c == KEY_RALT)
		return "KEY_RALT";
	if (c == KEY_SPACE)
		return "KEY_SPACE";
	if (c == KEY_CAPSLOCK)
		return "KEY_CAPSLOCK";
	if (c == KEY_F1)
		return "KEY_F1";
	if (c == KEY_F2)
		return "KEY_F2";
	if (c == KEY_F3)
		return "KEY_F3";
	if (c == KEY_F4)
		return "KEY_F4";
	if (c == KEY_F5)
		return "KEY_F5";
	if (c == KEY_F6)
		return "KEY_F6";
	if (c == KEY_F7)
		return "KEY_F7";
	if (c == KEY_F8)
		return "KEY_F8";
	if (c == KEY_F9)
		return "KEY_F9";
	if (c == KEY_F10)
		return "KEY_F10";
	if (c == KEY_KP_NUMLOCK)
		return "KEY_KP_NUMLOCK";
	if (c == KEY_SCROLLLOCK)
		return "KEY_SCROLLLOCK";
	if (c == KEY_HOME)
		return "KEY_HOME";
	if (c == KEY_KP_8)
		return "KEY_KP_8";
	if (c == KEY_PAGEUP)
		return "KEY_PAGEUP";
	if (c == KEY_KP_2)
		return "KEY_KP_2";
	if (c == KEY_KP_3)
		return "KEY_KP_3";
	if (c == KEY_KP_0)
		return "KEY_KP_0";
	if (c == KEY_KP_DECIMAL)
		return "KEY_KP_DECIMAL";
	if (c == KEY_UNKNOWN)
		return "KEY_UNKNOWN";
	if (c == KEY_UNKNOWN)
		return "KEY_UNKNOWN";
	if (c == KEY_UNKNOWN)
		return "KEY_UNKNOWN";
	if (c == KEY_F11)
		return "KEY_F11";
	if (c == KEY_F12)
		return "KEY_F12";
	return "ERROR0";
}
