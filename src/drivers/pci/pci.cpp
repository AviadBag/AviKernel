#include "drivers/pci/pci.h"
#include "drivers/pci/pci_device.h"
#include "drivers/serial_ports/serial_ports.h"

#include <cstdio.h>

#define PCI_NUMBER_OF_BUSES 256
#define PCI_NUMBER_OF_DEVICES_PER_BUS 32
#define PCI_NUMBER_OF_FUNCTIONS_PER_DEVICE 8

#define PCI_CONFIG_ADDRESS_PORT 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_HAS_MULTIPLE_FUNCTIONS_MASK 0x80       // When bit 7 of the header type is on (0x80), then this device has multiple functions.
#define PCI_VENDOR_ID_DEVICE_DOES_NOT_EXIST 0xFFFF // When the vendor id is this, then this device does not exist.

enum PCI_FIELDS_OFFSET // The offset of the 32 bit bundle contains this field.
{
    PCI_VENDOR_ID_OFFSET = 0x0,
    PCI_DEVICE_ID_OFFSET = 0x0,
    PCI_CLASS_CODE_OFFSET = 0x8,
    PCI_SUB_CLASS_CODE_OFFSET = 0x8,
    PCI_HEADER_TYPE_OFFSET = 0xC
};

enum PCI_FIELDS_MASK // The mask in order to extract a field from a 32 bit bundle.
{
    PCI_VENDOR_ID_MASK = 0x0000FFFF, // The lower 16 bits
    PCI_DEVICE_ID_MASK = 0xFFFF0000, // The higher 16 bits
    PCI_CLASS_CODE_MASK = 0x00FF0000,
    PCI_SUB_CLASS_CODE_MASK = 0xFF000000,
    PCI_HEADER_TYPE_MASK = 0x00FF0000
};

enum PCI_FIELDS_SHIFT // How many do I have to shift after masking?
{
    PCI_VENDOR_ID_SHIFT = 0,
    PCI_DEVICE_ID_SHIFT = 16,
    PCI_CLASS_CODE_SHIFT = 16,
    PCI_SUB_CLASS_CODE_SHIFT = 24,
    PCI_HEADER_TYPE_SHIFT = 24
};

Vector<PCIDevice> PCI::devices_vector;

void PCI::initialize()
{
    kprintf("Initializing PCI driver...\n");

    for (int bus = 0; bus < PCI_NUMBER_OF_BUSES; bus++)
        for (int device = 0; device < PCI_NUMBER_OF_DEVICES_PER_BUS; device++)
            check_device(bus, device);
}

Vector<PCIDevice>* PCI::get_devices_vector() 
{
    return &devices_vector;
}

void PCI::check_device(uint8_t bus, uint8_t device)
{
    uint16_t vendor_id = get_vendor_id(bus, device, 0);
    if (vendor_id == PCI_VENDOR_ID_DEVICE_DOES_NOT_EXIST)
        return;
    
    uint8_t header_type = get_header_type(bus, device, 0);
    // TODO: Support only header type 0!
    if (header_type & PCI_HAS_MULTIPLE_FUNCTIONS_MASK)
    {
        for (int function = 0; function < PCI_NUMBER_OF_FUNCTIONS_PER_DEVICE; function++)
            add_function(bus, device, function);
    }
    else
        add_function(bus, device, 0);
}

void PCI::add_function(uint8_t bus, uint8_t device, uint8_t function)
{
    uint8_t class_code = get_class_code(bus, device, function);
    uint8_t sub_class_code = get_sub_class_code(bus, device, function);

    PCIDevice d(class_code, sub_class_code, bus, device, function);
    devices_vector.append(d);
}

uint32_t PCI::pci_config_read_32_bits(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t bus_32 = (uint32_t)bus;
    uint32_t device_32 = (uint32_t)device;
    uint32_t function_32 = (uint32_t)function;
    uint32_t offset_32 = (uint32_t)offset;

    uint32_t address = 0x80000000;
    address |= (bus_32 << 16);
    address |= (device_32 << 11);
    address |= (function_32 << 8);
    address |= offset_32;

    SerialPorts::outl(PCI_CONFIG_ADDRESS_PORT, address);
    return SerialPorts::inl(PCI_CONFIG_DATA);
}

uint16_t PCI::get_vendor_id(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_config_read_32_bits(bus, device, function, PCI_VENDOR_ID_OFFSET) & PCI_VENDOR_ID_MASK) >> PCI_VENDOR_ID_SHIFT;
}

uint16_t PCI::get_device_id(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_config_read_32_bits(bus, device, function, PCI_DEVICE_ID_OFFSET) & PCI_DEVICE_ID_MASK) >> PCI_DEVICE_ID_SHIFT;
}

uint8_t PCI::get_sub_class_code(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_config_read_32_bits(bus, device, function, PCI_CLASS_CODE_OFFSET) & PCI_CLASS_CODE_MASK) >> PCI_CLASS_CODE_SHIFT;
}

uint8_t PCI::get_class_code(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_config_read_32_bits(bus, device, function, PCI_SUB_CLASS_CODE_OFFSET) & PCI_SUB_CLASS_CODE_MASK) >> PCI_SUB_CLASS_CODE_SHIFT;
}

uint8_t PCI::get_header_type(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_config_read_32_bits(bus, device, function, PCI_HEADER_TYPE_OFFSET) & PCI_HEADER_TYPE_MASK) >> PCI_HEADER_TYPE_SHIFT;
}
