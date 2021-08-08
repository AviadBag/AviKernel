#ifndef _BUS_PCI_DRIVER_H__
#define _BUS_PCI_DRIVER_H__

#include "hal/drivers/bus/bus_driver.h"

class BusPCIDriver : public BusDriver
{
public:
    virtual ~BusPCIDriver() {}

    virtual void detach() override;
    virtual bool exist () override;
    virtual void enumerate_devices() override;

private:
    void check_device(uint8_t bus, uint8_t device);
    void add_function(uint8_t bus, uint8_t device, uint8_t function);

    uint32_t pci_config_read_32_bits(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
    uint16_t get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
    uint16_t get_device_id(uint8_t bus, uint8_t device, uint8_t function);
    uint8_t  get_class_code(uint8_t bus, uint8_t device, uint8_t function);
    uint8_t  get_sub_class_code(uint8_t bus, uint8_t device, uint8_t function);
    uint8_t  get_header_type(uint8_t bus, uint8_t device, uint8_t function);
};

#endif // _BUS_PCI_DRIVER_H__