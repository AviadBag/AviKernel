#include "hal/drivers/pci/device.h"

Device::Device(uint8_t _bus_number, uint8_t _device_number, uint8_t _function_number)
{
    bus_number = _bus_number;
    device_number = _device_number;
    function_number = _function_number;
}

uint8_t Device::get_bus_number() const
{
    return bus_number;
}

uint8_t Device::get_device_number() const
{
    return device_number;
}

uint8_t Device::get_function_number() const
{
    return function_number;
}

/*const char* Device::type_to_string() const // Taken from https://github.com/MandelbrotOS/MandelbrotOS/blob/master/src/pci/pci_descriptors.c
{
    switch (class_code) {
    case 0:
        return "Unknown";
    case 1: {
        switch (sub_class_code) {
        case 0:
            return "SCSI storage controller";
        case 1:
            return "IDE interface";
        case 2:
            return "Floppy disk controller";
        case 3:
            return "IPI bus controller";
        case 4:
            return "RAID bus controller";
        case 5:
            return "ATA controller";
        case 6:
            return "SATA controller";
        case 7:
            return "Serial Attached SCSI controller";
        case 8:
            return "Non-Volatile memory controller";
        default:
            return "Mass Storage Controller";
        }
    }

    case 2: {
        switch (sub_class_code) {
        case 0:
            return "Ethernet controller";
        case 1:
            return "Token ring network controller";
        case 2:
            return "FDDI network controller";
        case 3:
            return "ATM network controller";
        case 4:
            return "ISDN controller";
        case 5:
            return "WorldFip controller";
        case 6:
            return "PICMG controller";
        case 7:
            return "Infiniband controller";
        case 8:
            return "Fabric controller";
        default:
            return "Network controller";
        }
    }

    case 3:
        switch (sub_class_code) {
        case 0:
            return "VGA compatible controller";
        case 1:
            return "XGA compatible controller";
        case 2:
            return "3D controller";
        default:
            return "Display controller";
        }
    case 4:
        switch (sub_class_code) {
        case 0:
            return "Multimedia video controller";
        case 1:
            return "Multimedia audio controller";
        case 2:
            return "Computer telephony device";
        case 3:
            return "Audio device";
        default:
            return "Multimedia controller";
        }
    case 5:
        switch (sub_class_code) {
        case 0:
            return "RAM memory";
        case 1:
            return "FLASH memory";
        default:
            return "Memory controller";
        }
    case 6:
        switch (sub_class_code) {
        case 0:
            return "Host bridge";
        case 1:
            return "ISA bridge";
        case 2:
            return "EISA bridge";
        case 3:
            return "MicroChannel bridge";
        case 4:
            return "PCI bridge";
        case 5:
            return "PCMCIA bridge";
        case 6:
            return "NuBus bridge";
        case 7:
            return "CardBus bridge";
        case 8:
            return "RACEway bridge";
        case 9:
            return "Semi-transparent PCI-to-PCI bridge";
        case 0xa:
            return "InfiniBand to PCI host bridge";
        default:
            return "Bridge";
        }
    case 7:
        switch (sub_class_code) {
        case 0:
            return "Serial controller";
        case 1:
            return "Parallel controller";
        case 2:
            return "Multiport serial controller";
        case 3:
            return "Modem";
        case 4:
            return "GPIB controller";
        case 5:
            return "Smard Card controller";
        default:
            return "Communication controller";
        }
    case 8:
        switch (sub_class_code) {
        case 0:
            return "PIC";
        case 1:
            return "DMA controller";
        case 2:
            return "Timer";
        case 3:
            return "RTC";
        case 4:
            return "PCI Hot-plug controller";
        case 5:
            return "SD Host controller";
        case 6:
            return "IOMMU";
        default:
            return "System peripheral";
        }
    case 9:
        switch (sub_class_code) {
        case 0:
            return "Keyboard controller";
        case 1:
            return "Digitizer Pen";
        case 2:
            return "Mouse controller";
        case 3:
            return "Scanner controller";
        case 4:
            return "Gameport controller";
        default:
            return "Input device controller";
        }
    case 0xa:
        switch (sub_class_code) {
        case 0:
            return "Generic Docking Station";
        default:
            return "Docking Station";
        }
    case 0xb:
        switch (sub_class_code) {
        case 0:
            return "386";
        case 1:
            return "486";
        case 2:
            return "Pentium";
        case 10:
            return "Alpha";
        case 20:
            return "Power PC";
        case 30:
            return "MIPS";
        case 40:
            return "Co-processor";
        default:
            return "Processor";
        }
    case 0xc:
        switch (sub_class_code) {
        case 0:
            return "FireWire (IEEE 1394)";
        case 1:
            return "ACCESS Bus";
        case 2:
            return "SSA";
        case 3:
            return "USB controller";
        case 4:
            return "Fibre Channel";
        case 5:
            return "SMBus";
        case 6:
            return "InfiniBand";
        case 7:
            return "IPMI Interface";
        case 8:
            return "SERCOS interface";
        case 9:
            return "CANBUS";
        default:
            return "Serial bus controller";
        }
    case 0xd:
        switch (sub_class_code) {
        case 0:
            return "IRDA controller";
        case 1:
            return "Consumer IR controller";
        case 10:
            return "RF controller";
        case 11:
            return "Bluetooth";
        case 12:
            return "Broadband";
        case 20:
            return "802.1a controller";
        case 21:
            return "802.1b controller";
        default:
            return "Wireless controller";
        }
    case 0xe:
        switch (sub_class_code) {
        case 0:
            return "I2O";
        default:
            return "Intelligent controller";
        }
    default:
        return "Unknown device";
    }
}

uint8_t Device::get_bus_number() const
{
    return bus_number;
}

uint8_t Device::get_device_number() const
{
    return device_number;
}

uint8_t Device::get_function_number() const
{
    return function_number;
}

uint8_t Device::get_class_code() const
{
    return class_code;
}

uint8_t Device::get_sub_class_code() const
{
    return sub_class_code;
}*/