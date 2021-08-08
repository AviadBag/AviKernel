#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>

/* This class represents a BUS device. It does not hold any useful information to the outer functions; It should be 
passed to the Driver in order to get the required data. */

class Device
{
public:
    Device() {}; // Required for using vector<Device>. Not sure why.
    Device(uint8_t bus_number, uint8_t device_number, uint8_t function_number);

    uint8_t get_bus_number() const;
    uint8_t get_device_number() const;
    uint8_t get_function_number() const;

private:
    uint8_t bus_number, device_number, function_number;
};

#endif // __DEVICE_H__