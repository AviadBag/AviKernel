#ifndef _BUS_PCI_DRIVER_H__
#define _BUS_PCI_DRIVER_H__

#include "drivers_manager/drivers/driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"

#include "utils/vector.h"

#include <stdint.h>

class PCIDriver : public Driver {
public:
    virtual void setup_driver_and_device() override;
    virtual bool exist() override;

    Vector<PCIDevice>* get_devices();

    uint16_t get_vendor_id(PCIDevice d);
    uint16_t get_device_id(PCIDevice d);
    uint8_t get_class_code(PCIDevice d);
    uint8_t get_sub_class_code(PCIDevice d);
    uint8_t get_header_type(PCIDevice d);
    uint8_t get_prog_if(PCIDevice d);
    const char* get_device_description(PCIDevice d);

    void set_prog_if(PCIDevice d, uint8_t prog_if);

private:
    void add_device(PCIDevice d);
    bool device_exists(PCIDevice d);
    void enumerate_devices();

    uint32_t generate_pci_config_address(PCIDevice d, uint8_t offset);
    uint32_t pci_config_read_32_bits(PCIDevice d, uint8_t offset);
    void pci_config_write_32_bits(PCIDevice d, uint8_t offset, uint32_t what);

    Vector<PCIDevice> devices;
};

#endif // _BUS_PCI_DRIVER_H__