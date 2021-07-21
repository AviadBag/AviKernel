#ifndef __PCI_DEVICE_H__
#define __PCI_DEVICE_H__

#include <stdint.h>

class PCIDevice
{
public:
    PCIDevice() {};
    PCIDevice(uint8_t class_code, uint8_t sub_class_code, uint8_t bus_number, uint8_t device_number, uint8_t function_number);

    const char *type_to_string() const;

    uint8_t get_class_code() const;
    uint8_t get_sub_class_code() const;

    uint8_t get_bus_number() const;
    uint8_t get_device_number() const;
    uint8_t get_function_number() const;

private:
    uint8_t class_code, sub_class_code;
    uint8_t bus_number, device_number, function_number;
};

#endif // __PCI_DEVICE_H__