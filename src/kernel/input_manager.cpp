#include "kernel/input_manager.h"
#include "drivers_manager/drivers/keyboard/keyboard_driver.h"
#include "drivers_manager/drivers_manager.h"

Queue<char> InputManager::chars_queue;

void InputManager::initialize()
{
    KeyboardDriver* keyboard_driver = (KeyboardDriver*)DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_KEYBOARD_DRIVER);
    keyboard_driver->set_on_press_listener(on_press);
}

void InputManager::on_press(ExtendedChar ec)
{
    if (ec.printable())
        chars_queue.enqueue(ec.as_regular_char());
}

char InputManager::getchar()
{
    while (chars_queue.empty())
        ;

    return chars_queue.dequeue();
}