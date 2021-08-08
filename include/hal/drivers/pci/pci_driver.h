#ifndef _BUS_PCI_DRIVER_H__
#define _BUS_PCI_DRIVER_H__

#include "hal/drivers/driver.h"
#include "hal/drivers/pci/device.h"

#include "utils/vector.h"

#include <stdint.h>

class PCIDriver : public Driver
{
public:
    virtual void attach() override;
    virtual void detach() override;
    virtual bool exist () override;
    void enumerate_devices();
    Vector<Device>* get_devices();

private:
    void check_device(uint8_t bus, uint8_t device);
    void add_device_to_list(Device d);

    uint32_t pci_config_read_32_bits(Device d, uint8_t offset);
    uint16_t get_vendor_id(Device d);
    uint16_t get_device_id(Device d);
    uint8_t  get_class_code(Device d);
    uint8_t  get_sub_class_code(Device d);
    uint8_t  get_header_type(Device d);

    Vector<Device> devices;
};

#endif // _BUS_PCI_DRIVER_H__