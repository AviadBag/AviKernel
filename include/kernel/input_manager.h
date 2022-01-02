#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "utils/queue.h"
#include "drivers_manager/drivers/keyboard/keyboard_driver.h"

// Manages all of the input stuff, using the keyboard driver. This is not a singleton, because we are using a listener here.

class InputManager
{
public:
    static void initialize();
    static char getchar();

private:
    static void on_press(ExtendedChar ec);

private:
    static Queue<char> chars_queue;
};

#endif // __INPUT_MANAGER_H__