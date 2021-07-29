#ifndef __HAL_H__
#define __HAL_H__

#include "hal/drivers/driver.h"

enum HAL_GENERAL_DRIVER_TYPE
{
    HAL_KEYBOARD_DRIVER,
    HAL_SCREEN_TEXT_DRIVER
};

#define HAL_NUMBER_OF_GENERAL_DRIVERS 2

class HAL
{
public:
    static HAL* get_instance();

    void initialize();
    Driver* get_driver(HAL_GENERAL_DRIVER_TYPE); // Returns nullptr if the given device does not exist

    HAL(HAL &other) = delete; // Should not be copied
    void operator=(const HAL &) = delete; // Should not be assigned

private:
    HAL(); // It's a singleton; It's constructor should be private.

    // I make it Driver* and not Driver - because I need polymorphism here. Aviad, just think about it a few seconds..
    Driver* general_drivers[HAL_NUMBER_OF_GENERAL_DRIVERS];

    static HAL* instance;
};

#endif // __HAL_H__