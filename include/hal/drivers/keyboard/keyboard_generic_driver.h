#ifndef _KEYBOARD_GENERIC_DRIVER_H__
#define _KEYBOARD_GENERIC_DRIVER_H__

#include "hal/drivers/keyboard/keyboard_driver.h"

class KeyboardGenericDriver : public KeyboardDriver
{
public:
    KeyboardGenericDriver();
    virtual ~KeyboardGenericDriver();

    virtual void attach();
    virtual void detach();
    virtual bool exist();

    virtual void set_on_press_listener(keyboard_on_press_listener l);
    virtual void set_on_release_listener(keyboard_on_release_listener l);

private:
    KeyboardDriver* driver = nullptr;
    KeyboardDriver* drivers[1]; // There is only one keyboard driver currently.
};

#endif // _KEYBOARD_GENERIC_DRIVER_H__