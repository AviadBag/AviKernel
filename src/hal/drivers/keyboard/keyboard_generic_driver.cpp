#include "hal/drivers/keyboard/keyboard_generic_driver.h"
#include "hal/drivers/keyboard/keyboard_driver.h"
#include "hal/drivers/keyboard/keyboard_ps2_driver.h"
#include "utils/general.h"

#include <stddef.h>

KeyboardGenericDriver::KeyboardGenericDriver()
    : KeyboardDriver()
{
    // Creates the drivers list
    drivers[0] = new KeyboardPS2Driver();
}

KeyboardGenericDriver::~KeyboardGenericDriver()
{
    for (size_t i = 0; i < ARRAY_LENGTH(drivers); i++)
        delete drivers[i];
}

void KeyboardGenericDriver::attach()
{
    for (size_t i = 0; i < ARRAY_LENGTH(drivers); i++) {
        if (drivers[i]->exist())
            driver = drivers[i];
    }
    driver->attach();
}

void KeyboardGenericDriver::set_on_press_listener(keyboard_on_press_listener l)
{
    KeyboardDriver::set_on_press_listener(l);
    driver->set_on_press_listener(l);
}

void KeyboardGenericDriver::set_on_release_listener(keyboard_on_release_listener l)
{
    KeyboardDriver::set_on_release_listener(l);
    driver->set_on_release_listener(l);
}

void KeyboardGenericDriver::detach()
{
    driver->detach();
}

bool KeyboardGenericDriver::exist()
{
    for (size_t i = 0; i < ARRAY_LENGTH(drivers); i++) {
        if (drivers[i]->exist())
            return true;
    }
    return false;
}
