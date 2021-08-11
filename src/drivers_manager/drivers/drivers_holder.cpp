#include "drivers_manager/drivers/drivers_holder.h"

void DriversHolder::add_driver(Driver* d) 
{
    all_drivers.append(d);
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
