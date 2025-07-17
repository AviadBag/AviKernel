#include "drivers_manager/drivers/keyboard/extended_char.h"

// Using the scan_code as an index, returns the corresponding avi_charcode.
// The array works only with make codes; break codes should be
static avi_charcode one_scan_code_to_avi_charcode_arr[] = {
    //! key			scancode
    KEY_UNKNOWN, // 0
    KEY_ESCAPE, // 1
    KEY_1, // 2
    KEY_2, // 3
    KEY_3, // 4
    KEY_4, // 5
    KEY_5, // 6
    KEY_6, // 7
    KEY_7, // 8
    KEY_8, // 9
    KEY_9, // 0xa
    KEY_0, // 0xb
    KEY_MINUS, // 0xc
    KEY_EQUAL, // 0xd
    KEY_BACKSPACE, // 0xe
    KEY_TAB, // 0xf
    KEY_Q, // 0x10
    KEY_W, // 0x11
    KEY_E, // 0x12
    KEY_R, // 0x13
    KEY_T, // 0x14
    KEY_Y, // 0x15
    KEY_U, // 0x16
    KEY_I, // 0x17
    KEY_O, // 0x18
    KEY_P, // 0x19
    KEY_LEFTBRACKET, // 0x1a
    KEY_RIGHTBRACKET, // 0x1b
    KEY_RETURN, // 0x1c
    KEY_LCTRL, // 0x1d
    KEY_A, // 0x1e
    KEY_S, // 0x1f
    KEY_D, // 0x20
    KEY_F, // 0x21
    KEY_G, // 0x22
    KEY_H, // 0x23
    KEY_J, // 0x24
    KEY_K, // 0x25
    KEY_L, // 0x26
    KEY_SEMICOLON, // 0x27
    KEY_QUOTE, // 0x28
    KEY_GRAVE, // 0x29
    KEY_LSHIFT, // 0x2a
    KEY_BACKSLASH, // 0x2b
    KEY_Z, // 0x2c
    KEY_X, // 0x2d
    KEY_C, // 0x2e
    KEY_V, // 0x2f
    KEY_B, // 0x30
    KEY_N, // 0x31
    KEY_M, // 0x32
    KEY_COMMA, // 0x33
    KEY_DOT, // 0x34
    KEY_SLASH, // 0x35
    KEY_RSHIFT, // 0x36
    KEY_KP_ASTERISK, // 0x37
    KEY_RALT, // 0x38
    KEY_SPACE, // 0x39
    KEY_CAPSLOCK, // 0x3a
    KEY_F1, // 0x3b
    KEY_F2, // 0x3c
    KEY_F3, // 0x3d
    KEY_F4, // 0x3e
    KEY_F5, // 0x3f
    KEY_F6, // 0x40
    KEY_F7, // 0x41
    KEY_F8, // 0x42
    KEY_F9, // 0x43
    KEY_F10, // 0x44
    KEY_KP_NUMLOCK, // 0x45
    KEY_SCROLLLOCK, // 0x46
    KEY_HOME, // 0x47
    KEY_KP_8, // 0x48	//keypad up arrow
    KEY_PAGEUP, // 0x49
    KEY_UNKNOWN, // 0x4a
    KEY_UNKNOWN, // 0x4b
    KEY_UNKNOWN, // 0x4c
    KEY_UNKNOWN, // 0x4d
    KEY_UNKNOWN, // 0x4e
    KEY_UNKNOWN, // 0x4f
    KEY_KP_2, // 0x50	//keypad down arrow
    KEY_KP_3, // 0x51	//keypad page down
    KEY_KP_0, // 0x52	//keypad insert key
    KEY_KP_DECIMAL, // 0x53	//keypad delete key
    KEY_UNKNOWN, // 0x54
    KEY_UNKNOWN, // 0x55
    KEY_UNKNOWN, // 0x56
    KEY_F11, // 0x57
    KEY_F12 // 0x58
};

static avi_charcode two_scan_codes_to_avi_charcode_arr[] = {
    //! key		scancode
    KEY_UNKNOWN, // 0
    KEY_UNKNOWN, // 1
    KEY_UNKNOWN, // 2
    KEY_UNKNOWN, // 3
    KEY_UNKNOWN, // 4
    KEY_UNKNOWN, // 5
    KEY_UNKNOWN, // 6
    KEY_UNKNOWN, // 7
    KEY_UNKNOWN, // 8
    KEY_UNKNOWN, // 9
    KEY_UNKNOWN, // 0xa
    KEY_UNKNOWN, // 0xb
    KEY_UNKNOWN, // 0xc
    KEY_UNKNOWN, // 0xd
    KEY_UNKNOWN, // 0xe
    KEY_UNKNOWN, // 0xf
    KEY_UNKNOWN, // 0x10
    KEY_UNKNOWN, // 0x11
    KEY_UNKNOWN, // 0x12
    KEY_UNKNOWN, // 0x13
    KEY_UNKNOWN, // 0x14
    KEY_UNKNOWN, // 0x15
    KEY_UNKNOWN, // 0x16
    KEY_UNKNOWN, // 0x17
    KEY_UNKNOWN, // 0x18
    KEY_UNKNOWN, // 0x19
    KEY_UNKNOWN, // 0x1a
    KEY_UNKNOWN, // 0x1b
    KEY_KP_ENTER, // 0x1c
    KEY_RCTRL, // 0x1d
    KEY_UNKNOWN, // 0x1e
    KEY_UNKNOWN, // 0x1f
    KEY_UNKNOWN, // 0x20
    KEY_UNKNOWN, // 0x21
    KEY_UNKNOWN, // 0x22
    KEY_UNKNOWN, // 0x23
    KEY_UNKNOWN, // 0x24
    KEY_UNKNOWN, // 0x25
    KEY_UNKNOWN, // 0x26
    KEY_UNKNOWN, // 0x27
    KEY_UNKNOWN, // 0x28
    KEY_UNKNOWN, // 0x29
    KEY_UNKNOWN, // 0x2a
    KEY_UNKNOWN, // 0x2b
    KEY_UNKNOWN, // 0x2c
    KEY_UNKNOWN, // 0x2d
    KEY_UNKNOWN, // 0x2e
    KEY_UNKNOWN, // 0x2f
    KEY_UNKNOWN, // 0x30
    KEY_UNKNOWN, // 0x31
    KEY_UNKNOWN, // 0x32
    KEY_UNKNOWN, // 0x33
    KEY_UNKNOWN, // 0x34
    KEY_KP_DIVIDE, // 0x35
    KEY_UNKNOWN, // 0x36
    KEY_UNKNOWN, // 0x37
    KEY_RALT, // 0x38
    KEY_UNKNOWN, // 0x39
    KEY_UNKNOWN, // 0x3a
    KEY_UNKNOWN, // 0x3b
    KEY_UNKNOWN, // 0x3c
    KEY_UNKNOWN, // 0x3d
    KEY_UNKNOWN, // 0x3e
    KEY_UNKNOWN, // 0x3f
    KEY_UNKNOWN, // 0x40
    KEY_UNKNOWN, // 0x41
    KEY_UNKNOWN, // 0x42
    KEY_UNKNOWN, // 0x43
    KEY_UNKNOWN, // 0x44
    KEY_UNKNOWN, // 0x45
    KEY_UNKNOWN, // 0x46
    KEY_HOME, // 0x47
    KEY_UPARROW, // 0x48
    KEY_PAGEUP, // 0x49
    KEY_UNKNOWN, // 0x4a
    KEY_LEFTARROW, // 0x4b
    KEY_UNKNOWN, // 0x4c
    KEY_RIGHTARROW, // 0x4d
    KEY_UNKNOWN, // 0x4e
    KEY_END, // 0x4f
    KEY_DOWNARROW, // 0x50
    KEY_PAGEDOWN, // 0x51
    KEY_INSERT, // 0x52
    KEY_DELETE, // 0x53
    KEY_UNKNOWN, // 0x54
    KEY_UNKNOWN, // 0x55
    KEY_UNKNOWN, // 0x56
    KEY_UNKNOWN, // 0x57
    KEY_UNKNOWN, // 0x58
    KEY_UNKNOWN, // 0x59
    KEY_UNKNOWN, // 0x5a
    KEY_LGUI, // 0x5b
    KEY_RGUI, // 0x5c
    KEY_APPS, // 0x5d
};

ExtendedChar::ExtendedChar()
{
    _avi_charcode = KEY_SPACE;
}

ExtendedChar::ExtendedChar(scan_code _scan_code)
{
    _avi_charcode = one_scan_code_to_avi_charcode_arr[_scan_code];
}

ExtendedChar::ExtendedChar([[gnu::unused]] scan_code _scan_code_1, scan_code _scan_code_2)
{
    /* I ignore _scan_code_1; It is always 0xE0. */
    _avi_charcode = two_scan_codes_to_avi_charcode_arr[_scan_code_2];
}

bool ExtendedChar::printable()
{
    return _avi_charcode < 0x1000; // The avi_charcodes above 0x1000 are non-printable.
}

avi_charcode ExtendedChar::as_avi_charcode()
{
    return _avi_charcode;
}

char ExtendedChar::as_regular_char()
{
    if (!printable())
        return '\0';

    return (char)_avi_charcode;
}

const char* ExtendedChar::avi_charcode_to_string(avi_charcode c)
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
    if (c == KEY_F11)
        return "KEY_F11";
    if (c == KEY_F12)
        return "KEY_F12";
    if (c == KEY_KP_ENTER)
        return "KEY_KP_ENTER";
    if (c == KEY_RCTRL)
        return "KEY_RCTRL";
    if (c == KEY_KP_DIVIDE)
        return "KEY_KP_DIVIDE";
    if (c == KEY_RALT)
        return "KEY_RALT";
    if (c == KEY_HOME)
        return "KEY_HOME";
    if (c == KEY_UPARROW)
        return "KEY_UPARROW";
    if (c == KEY_PAGEUP)
        return "KEY_PAGEUP";
    if (c == KEY_LEFTARROW)
        return "KEY_LEFTARROW";
    if (c == KEY_RIGHTARROW)
        return "KEY_RIGHTARROW";
    if (c == KEY_END)
        return "KEY_END";
    if (c == KEY_DOWNARROW)
        return "KEY_DOWNARROW";
    if (c == KEY_PAGEDOWN)
        return "KEY_PAGEDOWN";
    if (c == KEY_INSERT)
        return "KEY_INSERT";
    if (c == KEY_DELETE)
        return "KEY_DELETE";
    if (c == KEY_LGUI)
        return "KEY_LGUI";
    if (c == KEY_RGUI)
        return "KEY_RGUI";
    if (c == KEY_APPS)
        return "KEY_APPS";
    return "ERROR0";
}
