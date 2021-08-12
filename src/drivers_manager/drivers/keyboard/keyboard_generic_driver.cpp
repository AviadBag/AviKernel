#include "drivers_manager/drivers/keyboard/keyboard_generic_driver.h"
#include "drivers_manager/drivers/keyboard/keyboard_driver.h"
#include "drivers_manager/drivers/keyboard/keyboard_ps2_driver.h"

#include <stddef.h>

KeyboardGenericDriver::KeyboardGenericDriver()
    : KeyboardDriver()
{
    // Creates the drivers list
    drivers_holder.add_driver(new KeyboardPS2Driver());

    // Start checking which driver exists - now!
    drivers_holder.filter();
}

KeyboardGenericDriver::~KeyboardGenericDriver()
{
    // Delete all of the drivers
    for (int i = 0; i < drivers_holder.get_all_drivers().size(); i++)
        delete drivers_holder.get_all_drivers().get(i);
}

void KeyboardGenericDriver::setup_driver_and_device()
{
    for (int i = 0; i < drivers_holder.get_existing_drivers().size(); i++)
    {
        Driver* d = drivers_holder.get_existing_drivers().get(i);
        d->setup_driver_and_device();
    }
}

void KeyboardGenericDriver::set_on_press_listener(keyboard_on_press_listener l)
{
    KeyboardDriver::set_on_press_listener(l);
    for (int i = 0; i < drivers_holder.get_existing_drivers().size(); i++)
    {
        KeyboardDriver* keyboard_driver = (KeyboardDriver*) drivers_holder.get_existing_drivers().get(i);
        keyboard_driver->set_on_press_listener(l);
    }
}

void KeyboardGenericDriver::set_on_release_listener(keyboard_on_release_listener l)
{
    KeyboardDriver::set_on_release_listener(l);
    KeyboardDriver::set_on_press_listener(l);
    for (int i = 0; i < drivers_holder.get_existing_drivers().size(); i++)
    {
        KeyboardDriver* keyboard_driver = (KeyboardDriver*) drivers_holder.get_existing_drivers().get(i);
        keyboard_driver->set_on_release_listener(l);
    }
}

bool KeyboardGenericDriver::exist()
{
    return drivers_holder.get_existing_drivers().size() > 0;
}
