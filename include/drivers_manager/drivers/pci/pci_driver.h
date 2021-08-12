#ifndef _BUS_PCI_DRIVER_H__
#define _BUS_PCI_DRIVER_H__

#include "drivers_manager/drivers/driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"

#include "utils/vector.h"

#include <stdint.h>

class PCIDriver : public Driver
{
public:
    virtual void setup_driver_and_device() override;
    virtual bool exist () override;

    Vector<PCIDevice>* get_devices();

    uint16_t get_vendor_id(PCIDevice d);
    uint16_t get_device_id(PCIDevice d);
    uint8_t  get_class_code(PCIDevice d);
    uint8_t  get_sub_class_code(PCIDevice d);
    uint8_t  get_header_type(PCIDevice d);
    uint8_t  get_prog_if(PCIDevice d);

private:
    void check_device(uint8_t bus, uint8_t device);
    void enumerate_devices();

    uint32_t pci_config_read_32_bits(PCIDevice d, uint8_t offset);

    Vector<PCIDevice> devices;
};

#endif // _BUS_PCI_DRIVER_H__