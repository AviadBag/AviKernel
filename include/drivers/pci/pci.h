#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>

class PCI {
public:
    static void initialize();

private:
    static void check_device(uint8_t bus, uint8_t device);
    static uint32_t pci_config_read_32_bits(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

    static uint16_t get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
    static uint16_t get_device_id(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_class_code(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_sub_class_code(uint8_t bus, uint8_t device, uint8_t function);
    static uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function);
};

#endif // __PCI_H__