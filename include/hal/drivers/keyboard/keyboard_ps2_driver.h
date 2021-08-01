#ifndef _KEYBOARD_PS2_DRIVER_H__
#define _KEYBOARD_PS2_DRIVER_H__

#include "hal/drivers/driver.h"
#include "hal/drivers/keyboard/extended_char.h"

#include <stdint.h>

typedef void (*keyboard_on_press_listener)(ExtendedChar);
typedef void (*keyboard_on_release_listener)(ExtendedChar);

class KeyboardPS2Driver : public Driver
{
public:
    virtual void attach();
    virtual void detach();

    void set_on_press_listener(keyboard_on_press_listener);
    void set_on_release_listener(keyboard_on_release_listener);

private:
    void on_ke_data(uint32_t unused);           // Called whenever the keyboard encoder sends an interrupt
    void on_make_scan_code(uint8_t scan_code);  // Called whenever there is a new make scan code
    void on_break_scan_code(uint8_t scan_code); // Called whenever there is a new break scan code
    bool ignore(uint8_t scan_code);             // Shall I ignore this scan code?
    uint8_t ke_read_data();                     // Reads a byte from the keyboard encoder
};

#endif // _KEYBOARD_PS2_DRIVER_H__