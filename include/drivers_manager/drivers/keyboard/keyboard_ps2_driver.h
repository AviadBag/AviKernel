#ifndef _KEYBOARD_PS2_DRIVER_H__
#define _KEYBOARD_PS2_DRIVER_H__

#include "drivers_manager/drivers/keyboard/keyboard_driver.h"

#include <stdint.h>

typedef void (*keyboard_on_press_listener)(ExtendedChar);
typedef void (*keyboard_on_release_listener)(ExtendedChar);

class KeyboardPS2Driver : public KeyboardDriver {
public:
    virtual void attach() override;
    virtual void detach() override;
    virtual bool exist () override;

private:
    void on_ke_data(); // Called whenever the keyboard encoder sends an interrupt
    void on_make_scan_code(uint8_t scan_code); // Called whenever there is a new make scan code
    void on_break_scan_code(uint8_t scan_code); // Called whenever there is a new break scan code
    bool ignore(uint8_t scan_code); // Shall I ignore this scan code?
    uint8_t ke_read_data(); // Reads a byte from the keyboard encoder

    bool wait_for_second_scan_code = false; // Shall I wait for a second scan code?
    bool wait_for_second_and_third_scan_code = false; // Shall I wait for a second and a third scan code?
};

#endif // _KEYBOARD_PS2_DRIVER_H__