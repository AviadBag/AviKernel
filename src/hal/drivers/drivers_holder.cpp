#include "hal/drivers/drivers_holder.h"
#include "kernel/panic.h"

void DriversHolder::add_driver(Driver* d) 
{
    if (!all_drivers.append(d))
        panic("DriversHolder: Error while trying to add a driver: Not enough memory!");
}

void DriversHolder::filter() 
{
    for (int i = 0; i < all_drivers.size(); i++)
    {
        Driver* d = all_drivers.get(i);
        if (d->exist())
            existing_drivers.append(d);
    }
}

Vector<Driver*> DriversHolder::get_existing_drivers() 
{
    return existing_drivers;
}

Vector<Driver*> DriversHolder::get_all_drivers() 
{
    return all_drivers;
}
