#ifndef _BUS_PCI_DRIVER_H__
#define _BUS_PCI_DRIVER_H__

#include "hal/drivers/bus/bus_driver.h"

class BusPCIDriver : public BusDriver
{
public:
    virtual void detach() override;
    virtual bool exist () override;
    virtual void enumerate_devices() override;

private:
    void check_device(uint8_t bus, uint8_t device);
    void add_device_to_list(Device d);

    uint32_t pci_config_read_32_bits(Device d, uint8_t offset);
    uint16_t get_vendor_id(Device d);
    uint16_t get_device_id(Device d);
    uint8_t  get_class_code(Device d);
    uint8_t  get_sub_class_code(Device d);
    uint8_t  get_header_type(Device d);
};

#endif // _BUS_PCI_DRIVER_H__