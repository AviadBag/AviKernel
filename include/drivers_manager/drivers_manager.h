#ifndef __DRIVERS_MANAGER_H__
#define __DRIVERS_MANAGER_H__

#include "drivers_manager/drivers/driver.h"

enum DRIVERS_MANAGER_GENERAL_DRIVER_TYPE
{
    DRIVERS_MANAGER_KEYBOARD_DRIVER,
    DRIVERS_MANAGER_CLOCK_DRIVER,
    DRIVERS_MANAGER_PCI_DRIVER,
    DRIVERS_MANAGER_PIC_DRIVER,
    DRIVERS_MANAGER_STORAGE_DRIVER,
    DRIVERS_MANAGER_VIDEO_DRIVER
    // Don't forget to update: DRIVERS_MANAGER_NUMBER_OF_GENERAL_DRIVERS!
};

#define DRIVERS_MANAGER_NUMBER_OF_GENERAL_DRIVERS 6

class DriversManager
{
public:
    void initialize();
    Driver *get_driver(DRIVERS_MANAGER_GENERAL_DRIVER_TYPE);

private:
    // I make it Driver* and not Driver - because I need polymorphism here. Aviad, just think about it a few seconds..
    Driver *general_drivers[DRIVERS_MANAGER_NUMBER_OF_GENERAL_DRIVERS];

    /* ------------------------- Singelton stuff. ------------------------- */
public:
    static DriversManager *get_instance();

    DriversManager(DriversManager &other) = delete; // Should not be cloneable.
    void operator=(const DriversManager &other) = delete;

private:
    DriversManager(); // This is a singelton - So a private constructor.

    static DriversManager *instance;
};

#endif // __DRIVERS_MANAGER_H__