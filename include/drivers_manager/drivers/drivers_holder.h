#ifndef __DRIVERS_VECTOR_H__
#define __DRIVERS_VECTOR_H__

#include "drivers_manager/drivers/driver.h"
#include "utils/vector.h"

#include <stdarg.h>

// This module holds drivers. You add drivers to it, and it only saves the drivers that exist, and lets you
// iterate over them.
// I implement it with two vectors - one for all of the drivers, one for the existing drivers. I save the unexisting
// drivers because I am planning in the feature to add Plug And Play support - so even the unexisting drivers should
// remain.

class DriversHolder {
public:
    void add_driver(Driver* d);
    void filter(); // Does the distinction between un existing driver and an unexisting driver.
    Vector<Driver*> get_all_drivers();
    Vector<Driver*> get_existing_drivers();

private:
    Vector<Driver*> all_drivers;
    Vector<Driver*> existing_drivers;
};

#endif // __DRIVERS_VECTOR_H__