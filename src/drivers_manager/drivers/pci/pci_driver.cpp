#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"
#include "utils/io.h"

#define PCI_NUMBER_OF_BUSES 256
#define PCI_NUMBER_OF_DEVICES_PER_BUS 32
#define PCI_NUMBER_OF_FUNCTIONS_PER_DEVICE 8

#define PCI_CONFIG_ADDRESS_PORT 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_HAS_MULTIPLE_FUNCTIONS_MASK 0x80 // When bit 7 of the header type is on (0x80), then this device has multiple functions.
#define PCI_VENDOR_ID_DEVICE_DOES_NOT_EXIST 0xFFFF // When the vendor id is this, then this device does not exist.

enum PCI_FIELDS_OFFSET // The offset of the 32 bit bundle contains this field.
{
    PCI_VENDOR_ID_OFFSET = 0x0,
    PCI_DEVICE_ID_OFFSET = 0x0,
    PCI_CLASS_CODE_OFFSET = 0x8,
    PCI_SUB_CLASS_CODE_OFFSET = 0x8,
    PCI_HEADER_TYPE_OFFSET = 0xC,
    PCI_PROG_IF_OFFSET = 0x8
};

enum PCI_FIELDS_MASK // The mask in order to extract a field from a 32 bit bundle.
{
    PCI_VENDOR_ID_MASK = 0x0000FFFF, // The lower 16 bits
    PCI_DEVICE_ID_MASK = 0xFFFF0000, // The higher 16 bits
    PCI_CLASS_CODE_MASK = 0xFF000000,
    PCI_SUB_CLASS_CODE_MASK = 0x00FF0000,
    PCI_HEADER_TYPE_MASK = 0x00FF0000,
    PCI_PROG_IF_MASK = 0x0000FF00
};

enum PCI_FIELDS_SHIFT // How many do I have to shift after masking?
{
    PCI_VENDOR_ID_SHIFT = 0,
    PCI_DEVICE_ID_SHIFT = 16,
    PCI_CLASS_CODE_SHIFT = 24,
    PCI_SUB_CLASS_CODE_SHIFT = 16,
    PCI_HEADER_TYPE_SHIFT = 16,
    PCI_PROG_IF_SHIFT = 8
};

void PCIDriver::setup_driver_and_device() 
{
    enumerate_devices();
}

Vector<PCIDevice>* PCIDriver::get_devices() 
{
    return &devices;
}

bool PCIDriver::exist() 
{
    return true; // TODO: Implement it.
}

void PCIDriver::enumerate_devices()
{
    for (int bus = 0; bus < PCI_NUMBER_OF_BUSES; bus++)
    {
        for (int device_number = 0; device_number < PCI_NUMBER_OF_DEVICES_PER_BUS; device_number++)
        {
            PCIDevice device(bus, device_number, 0);
            if (!device_exists(device))
                continue;

            add_device(device);
        }
    }
}

void PCIDriver::add_device(PCIDevice device)
{
    uint8_t header_type = get_header_type(device);
    // TODO: Support only header type 0!
    if (header_type & PCI_HAS_MULTIPLE_FUNCTIONS_MASK) {
        // Check every function of this device
        for (int function = 0; function < PCI_NUMBER_OF_FUNCTIONS_PER_DEVICE; function++)
        {
            PCIDevice d(device.get_bus_number(), device.get_device_number(), function);
            if (!device_exists(d))
                continue;
            devices.append(d);
        }
    } else
        devices.append(device);
}

bool PCIDriver::device_exists(PCIDevice d) 
{
    uint16_t vendor_id = get_vendor_id(d);
    return vendor_id != PCI_VENDOR_ID_DEVICE_DOES_NOT_EXIST;
}  

uint32_t PCIDriver::generate_pci_config_address(PCIDevice d, uint8_t offset) 
{
    uint32_t bus_32 = (uint32_t)d.get_bus_number();
    uint32_t device_32 = (uint32_t)d.get_device_number();
    uint32_t function_32 = (uint32_t)d.get_function_number();
    uint32_t offset_32 = (uint32_t)offset;

    uint32_t address = 0x80000000;
    address |= (bus_32 << 16);
    address |= (device_32 << 11);
    address |= (function_32 << 8);
    address |= offset_32;

    return address;
}

uint32_t PCIDriver::pci_config_read_32_bits(PCIDevice d, uint8_t offset)
{
    uint32_t address = generate_pci_config_address(d, offset);
    IO::outl(PCI_CONFIG_ADDRESS_PORT, address);
    return IO::inl(PCI_CONFIG_DATA);
}

void PCIDriver::pci_config_write_32_bits(PCIDevice d, uint8_t offset, uint32_t what) 
{
    uint32_t address = generate_pci_config_address(d, offset);
    IO::outl(PCI_CONFIG_ADDRESS_PORT, address);
    IO::outl(PCI_CONFIG_DATA, what);
}

uint16_t PCIDriver::get_vendor_id(PCIDevice d)
{
    return (pci_config_read_32_bits(d, PCI_VENDOR_ID_OFFSET) & PCI_VENDOR_ID_MASK) >> PCI_VENDOR_ID_SHIFT;
}

uint16_t PCIDriver::get_device_id(PCIDevice d)
{
    return (pci_config_read_32_bits(d, PCI_DEVICE_ID_OFFSET) & PCI_DEVICE_ID_MASK) >> PCI_DEVICE_ID_SHIFT;
}

uint8_t PCIDriver::get_class_code(PCIDevice d)
{
    return (pci_config_read_32_bits(d, PCI_CLASS_CODE_OFFSET) & PCI_CLASS_CODE_MASK) >> PCI_CLASS_CODE_SHIFT;
}

uint8_t PCIDriver::get_sub_class_code(PCIDevice d)
{
    return (pci_config_read_32_bits(d, PCI_SUB_CLASS_CODE_OFFSET) & PCI_SUB_CLASS_CODE_MASK) >> PCI_SUB_CLASS_CODE_SHIFT;
}

uint8_t PCIDriver::get_header_type(PCIDevice d)
{
    return (pci_config_read_32_bits(d, PCI_HEADER_TYPE_OFFSET) & PCI_HEADER_TYPE_MASK) >> PCI_HEADER_TYPE_SHIFT;
}

uint8_t PCIDriver::get_prog_if(PCIDevice d) 
{
    return (pci_config_read_32_bits(d, PCI_PROG_IF_OFFSET) & PCI_PROG_IF_MASK) >> PCI_PROG_IF_SHIFT;
}

void PCIDriver::set_prog_if(PCIDevice d, uint8_t prog_if) 
{
    pci_config_write_32_bits(d, PCI_PROG_IF_OFFSET, prog_if << PCI_PROG_IF_SHIFT);
}
