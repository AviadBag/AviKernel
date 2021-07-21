#ifndef __PCI_H__
#define __PCI_H__

#include "utils/vector.h"
#include "drivers/pci/pci_device.h"

#include <stdint.h>

class PCI {
public:
    static void initialize();
    static Vector<PCIDevice>* get_devices_vector();

private:
    static void check_device(uint8_t bus, uint8_t device);
    static void add_function(uint8_t bus, uint8_t device, uint8_t function);

    static uint32_t pci_config_read_32_bits(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
    static uint16_t get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
    static uint16_t get_device_id(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_class_code(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_sub_class_code(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function);

    static Vector<PCIDevice> devices_vector;
};

#endif // __PCI_H__