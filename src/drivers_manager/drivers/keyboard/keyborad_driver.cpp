#include "drivers_manager/drivers/keyboard/keyboard_driver.h"

void KeyboardDriver::set_on_press_listener(keyboard_on_press_listener l) { press_listener = l; }

void KeyboardDriver::set_on_release_listener(keyboard_on_release_listener l) { release_listener = l; }