#ifndef _KEYBOARD_DRIVER_H__
#define _KEYBOARD_DRIVER_H__

#include "drivers_manager/drivers/driver.h"
#include "drivers_manager/drivers/keyboard/extended_char.h"

// An abstract keyboard driver - defines the interface of every keyboard driver.

typedef void (*keyboard_on_press_listener)(ExtendedChar);
typedef void (*keyboard_on_release_listener)(ExtendedChar);

class KeyboardDriver : public Driver {
public:
    virtual void set_on_press_listener(keyboard_on_press_listener);
    virtual void set_on_release_listener(keyboard_on_release_listener);

protected:
    keyboard_on_press_listener press_listener = nullptr;
    keyboard_on_release_listener release_listener = nullptr;
};

#endif // _KEYBOARD_DRIVER_H__