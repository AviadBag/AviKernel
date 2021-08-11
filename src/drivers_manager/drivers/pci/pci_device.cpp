#include "drivers_manager/drivers/pci/pci_device.h"

PCIDevice::PCIDevice(uint8_t _bus_number, uint8_t _device_number, uint8_t _function_number)
{
    bus_number = _bus_number;
    device_number = _device_number;
    function_number = _function_number;
}

uint8_t PCIDevice::get_bus_number() const
{
    return bus_number;
}

uint8_t PCIDevice::get_device_number() const
{
    return device_number;
}

uint8_t PCIDevice::get_function_number() const
{
    return function_number;
}