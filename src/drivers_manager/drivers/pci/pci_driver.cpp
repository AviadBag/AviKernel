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
    for (int bus = 0; bus < PCI_NUMBER_OF_BUSES; bus++) {
        for (int device_number = 0; device_number < PCI_NUMBER_OF_DEVICES_PER_BUS; device_number++) {
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
        for (int function = 0; function < PCI_NUMBER_OF_FUNCTIONS_PER_DEVICE; function++) {
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

// Luckily was implemented by Claude..
const char* PCIDriver::get_device_description(PCIDevice d)
{
    uint8_t class_code = get_class_code(d);
    uint8_t sub_class = get_sub_class_code(d);
    uint8_t prog_if = get_prog_if(d);

    switch (class_code) {
    case 0x0: // Unclassified
        switch (sub_class) {
        case 0x0:
            return "Unclassified, Non-VGA-Compatible Unclassified Device";
        case 0x1:
            return "Unclassified, VGA-Compatible Unclassified Device";
        default:
            return "Unclassified, Unknown";
        }

    case 0x1: // Mass Storage Controller
        switch (sub_class) {
        case 0x0:
            return "Mass Storage Controller, SCSI Bus Controller";
        case 0x1: // IDE Controller
            switch (prog_if) {
            case 0x0:
                return "Mass Storage Controller, IDE Controller, ISA Compatibility mode-only controller";
            case 0x5:
                return "Mass Storage Controller, IDE Controller, PCI native mode-only controller";
            case 0xA:
                return "Mass Storage Controller, IDE Controller, ISA Compatibility mode controller, supports both channels switched to PCI native mode";
            case 0xF:
                return "Mass Storage Controller, IDE Controller, PCI native mode controller, supports both channels switched to ISA compatibility mode";
            case 0x80:
                return "Mass Storage Controller, IDE Controller, ISA Compatibility mode-only controller, supports bus mastering";
            case 0x85:
                return "Mass Storage Controller, IDE Controller, PCI native mode-only controller, supports bus mastering";
            case 0x8A:
                return "Mass Storage Controller, IDE Controller, ISA Compatibility mode controller, supports both channels switched to PCI native mode, supports bus mastering";
            case 0x8F:
                return "Mass Storage Controller, IDE Controller, PCI native mode controller, supports both channels switched to ISA compatibility mode, supports bus mastering";
            default:
                return "Mass Storage Controller, IDE Controller";
            }
        case 0x2:
            return "Mass Storage Controller, Floppy Disk Controller";
        case 0x3:
            return "Mass Storage Controller, IPI Bus Controller";
        case 0x4:
            return "Mass Storage Controller, RAID Controller";
        case 0x5: // ATA Controller
            switch (prog_if) {
            case 0x20:
                return "Mass Storage Controller, ATA Controller, Single DMA";
            case 0x30:
                return "Mass Storage Controller, ATA Controller, Chained DMA";
            default:
                return "Mass Storage Controller, ATA Controller";
            }
        case 0x6: // Serial ATA Controller
            switch (prog_if) {
            case 0x0:
                return "Mass Storage Controller, Serial ATA Controller, Vendor Specific Interface";
            case 0x1:
                return "Mass Storage Controller, Serial ATA Controller, AHCI 1.0";
            case 0x2:
                return "Mass Storage Controller, Serial ATA Controller, Serial Storage Bus";
            default:
                return "Mass Storage Controller, Serial ATA Controller";
            }
        case 0x7: // Serial Attached SCSI Controller
            switch (prog_if) {
            case 0x0:
                return "Mass Storage Controller, Serial Attached SCSI Controller, SAS";
            case 0x1:
                return "Mass Storage Controller, Serial Attached SCSI Controller, Serial Storage Bus";
            default:
                return "Mass Storage Controller, Serial Attached SCSI Controller";
            }
        case 0x8: // Non-Volatile Memory Controller
            switch (prog_if) {
            case 0x1:
                return "Mass Storage Controller, Non-Volatile Memory Controller, NVMHCI";
            case 0x2:
                return "Mass Storage Controller, Non-Volatile Memory Controller, NVM Express";
            default:
                return "Mass Storage Controller, Non-Volatile Memory Controller";
            }
        case 0x80:
            return "Mass Storage Controller, Other";
        default:
            return "Mass Storage Controller, Unknown";
        }

    case 0x2: // Network Controller
        switch (sub_class) {
        case 0x0:
            return "Network Controller, Ethernet Controller";
        case 0x1:
            return "Network Controller, Token Ring Controller";
        case 0x2:
            return "Network Controller, FDDI Controller";
        case 0x3:
            return "Network Controller, ATM Controller";
        case 0x4:
            return "Network Controller, ISDN Controller";
        case 0x5:
            return "Network Controller, WorldFip Controller";
        case 0x6:
            return "Network Controller, PICMG 2.14 Multi Computing Controller";
        case 0x7:
            return "Network Controller, Infiniband Controller";
        case 0x8:
            return "Network Controller, Fabric Controller";
        case 0x80:
            return "Network Controller, Other";
        default:
            return "Network Controller, Unknown";
        }

    case 0x3: // Display Controller
        switch (sub_class) {
        case 0x0: // VGA Compatible Controller
            switch (prog_if) {
            case 0x0:
                return "Display Controller, VGA Compatible Controller, VGA Controller";
            case 0x1:
                return "Display Controller, VGA Compatible Controller, 8514-Compatible Controller";
            default:
                return "Display Controller, VGA Compatible Controller";
            }
        case 0x1:
            return "Display Controller, XGA Controller";
        case 0x2:
            return "Display Controller, 3D Controller (Not VGA-Compatible)";
        case 0x80:
            return "Display Controller, Other";
        default:
            return "Display Controller, Unknown";
        }

    case 0x4: // Multimedia Controller
        switch (sub_class) {
        case 0x0:
            return "Multimedia Controller, Multimedia Video Controller";
        case 0x1:
            return "Multimedia Controller, Multimedia Audio Controller";
        case 0x2:
            return "Multimedia Controller, Computer Telephony Device";
        case 0x3:
            return "Multimedia Controller, Audio Device";
        case 0x80:
            return "Multimedia Controller, Other";
        default:
            return "Multimedia Controller, Unknown";
        }

    case 0x5: // Memory Controller
        switch (sub_class) {
        case 0x0:
            return "Memory Controller, RAM Controller";
        case 0x1:
            return "Memory Controller, Flash Controller";
        case 0x80:
            return "Memory Controller, Other";
        default:
            return "Memory Controller, Unknown";
        }

    case 0x6: // Bridge
        switch (sub_class) {
        case 0x0:
            return "Bridge, Host Bridge";
        case 0x1:
            return "Bridge, ISA Bridge";
        case 0x2:
            return "Bridge, EISA Bridge";
        case 0x3:
            return "Bridge, MCA Bridge";
        case 0x4: // PCI-to-PCI Bridge
            switch (prog_if) {
            case 0x0:
                return "Bridge, PCI-to-PCI Bridge, Normal Decode";
            case 0x1:
                return "Bridge, PCI-to-PCI Bridge, Subtractive Decode";
            default:
                return "Bridge, PCI-to-PCI Bridge";
            }
        case 0x5:
            return "Bridge, PCMCIA Bridge";
        case 0x6:
            return "Bridge, NuBus Bridge";
        case 0x7:
            return "Bridge, CardBus Bridge";
        case 0x8: // RACEway Bridge
            switch (prog_if) {
            case 0x0:
                return "Bridge, RACEway Bridge, Transparent Mode";
            case 0x1:
                return "Bridge, RACEway Bridge, Endpoint Mode";
            default:
                return "Bridge, RACEway Bridge";
            }
        case 0x9: // PCI-to-PCI Bridge
            switch (prog_if) {
            case 0x40:
                return "Bridge, PCI-to-PCI Bridge, Semi-Transparent, Primary bus towards host CPU";
            case 0x80:
                return "Bridge, PCI-to-PCI Bridge, Semi-Transparent, Secondary bus towards host CPU";
            default:
                return "Bridge, PCI-to-PCI Bridge";
            }
        case 0x0A:
            return "Bridge, InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Bridge, Other";
        default:
            return "Bridge, Unknown";
        }

    case 0x7: // Simple Communication Controller
        switch (sub_class) {
        case 0x0: // Serial Controller
            switch (prog_if) {
            case 0x0:
                return "Simple Communication Controller, Serial Controller, 8250-Compatible (Generic XT)";
            case 0x1:
                return "Simple Communication Controller, Serial Controller, 16450-Compatible";
            case 0x2:
                return "Simple Communication Controller, Serial Controller, 16550-Compatible";
            case 0x3:
                return "Simple Communication Controller, Serial Controller, 16650-Compatible";
            case 0x4:
                return "Simple Communication Controller, Serial Controller, 16750-Compatible";
            case 0x5:
                return "Simple Communication Controller, Serial Controller, 16850-Compatible";
            case 0x6:
                return "Simple Communication Controller, Serial Controller, 16950-Compatible";
            default:
                return "Simple Communication Controller, Serial Controller";
            }
        case 0x1: // Parallel Controller
            switch (prog_if) {
            case 0x0:
                return "Simple Communication Controller, Parallel Controller, Standard Parallel Port";
            case 0x1:
                return "Simple Communication Controller, Parallel Controller, Bi-Directional Parallel Port";
            case 0x2:
                return "Simple Communication Controller, Parallel Controller, ECP 1.X Compliant Parallel Port";
            case 0x3:
                return "Simple Communication Controller, Parallel Controller, IEEE 1284 Controller";
            case 0xFE:
                return "Simple Communication Controller, Parallel Controller, IEEE 1284 Target Device";
            default:
                return "Simple Communication Controller, Parallel Controller";
            }
        case 0x2:
            return "Simple Communication Controller, Multiport Serial Controller";
        case 0x3: // Modem
            switch (prog_if) {
            case 0x0:
                return "Simple Communication Controller, Modem, Generic Modem";
            case 0x1:
                return "Simple Communication Controller, Modem, Hayes 16450-Compatible Interface";
            case 0x2:
                return "Simple Communication Controller, Modem, Hayes 16550-Compatible Interface";
            case 0x3:
                return "Simple Communication Controller, Modem, Hayes 16650-Compatible Interface";
            case 0x4:
                return "Simple Communication Controller, Modem, Hayes 16750-Compatible Interface";
            default:
                return "Simple Communication Controller, Modem";
            }
        case 0x4:
            return "Simple Communication Controller, IEEE 488.1/2 (GPIB) Controller";
        case 0x5:
            return "Simple Communication Controller, Smart Card Controller";
        case 0x80:
            return "Simple Communication Controller, Other";
        default:
            return "Simple Communication Controller, Unknown";
        }

    case 0x8: // Base System Peripheral
        switch (sub_class) {
        case 0x0: // PIC
            switch (prog_if) {
            case 0x0:
                return "Base System Peripheral, PIC, Generic 8259-Compatible";
            case 0x1:
                return "Base System Peripheral, PIC, ISA-Compatible";
            case 0x2:
                return "Base System Peripheral, PIC, EISA-Compatible";
            case 0x10:
                return "Base System Peripheral, PIC, I/O APIC Interrupt Controller";
            case 0x20:
                return "Base System Peripheral, PIC, I/O(x) APIC Interrupt Controller";
            default:
                return "Base System Peripheral, PIC";
            }
        case 0x1: // DMA Controller
            switch (prog_if) {
            case 0x0:
                return "Base System Peripheral, DMA Controller, Generic 8237-Compatible";
            case 0x1:
                return "Base System Peripheral, DMA Controller, ISA-Compatible";
            case 0x2:
                return "Base System Peripheral, DMA Controller, EISA-Compatible";
            default:
                return "Base System Peripheral, DMA Controller";
            }
        case 0x2: // Timer
            switch (prog_if) {
            case 0x0:
                return "Base System Peripheral, Timer, Generic 8254-Compatible";
            case 0x1:
                return "Base System Peripheral, Timer, ISA-Compatible";
            case 0x2:
                return "Base System Peripheral, Timer, EISA-Compatible";
            case 0x3:
                return "Base System Peripheral, Timer, HPET";
            default:
                return "Base System Peripheral, Timer";
            }
        case 0x3: // RTC Controller
            switch (prog_if) {
            case 0x0:
                return "Base System Peripheral, RTC Controller, Generic RTC";
            case 0x1:
                return "Base System Peripheral, RTC Controller, ISA-Compatible";
            default:
                return "Base System Peripheral, RTC Controller";
            }
        case 0x4:
            return "Base System Peripheral, PCI Hot-Plug Controller";
        case 0x5:
            return "Base System Peripheral, SD Host controller";
        case 0x6:
            return "Base System Peripheral, IOMMU";
        case 0x80:
            return "Base System Peripheral, Other";
        default:
            return "Base System Peripheral, Unknown";
        }

    case 0x9: // Input Device Controller
        switch (sub_class) {
        case 0x0:
            return "Input Device Controller, Keyboard Controller";
        case 0x1:
            return "Input Device Controller, Digitizer Pen";
        case 0x2:
            return "Input Device Controller, Mouse Controller";
        case 0x3:
            return "Input Device Controller, Scanner Controller";
        case 0x4: // Gameport Controller
            switch (prog_if) {
            case 0x0:
                return "Input Device Controller, Gameport Controller, Generic";
            case 0x10:
                return "Input Device Controller, Gameport Controller, Extended";
            default:
                return "Input Device Controller, Gameport Controller";
            }
        case 0x80:
            return "Input Device Controller, Other";
        default:
            return "Input Device Controller, Unknown";
        }

    case 0xA: // Docking Station
        switch (sub_class) {
        case 0x0:
            return "Docking Station, Generic";
        case 0x80:
            return "Docking Station, Other";
        default:
            return "Docking Station, Unknown";
        }

    case 0xB: // Processor
        switch (sub_class) {
        case 0x0:
            return "Processor, 386";
        case 0x1:
            return "Processor, 486";
        case 0x2:
            return "Processor, Pentium";
        case 0x3:
            return "Processor, Pentium Pro";
        case 0x10:
            return "Processor, Alpha";
        case 0x20:
            return "Processor, PowerPC";
        case 0x30:
            return "Processor, MIPS";
        case 0x40:
            return "Processor, Co-Processor";
        case 0x80:
            return "Processor, Other";
        default:
            return "Processor, Unknown";
        }

    case 0xC: // Serial Bus Controller
        switch (sub_class) {
        case 0x0: // FireWire (IEEE 1394) Controller
            switch (prog_if) {
            case 0x0:
                return "Serial Bus Controller, FireWire (IEEE 1394) Controller, Generic";
            case 0x10:
                return "Serial Bus Controller, FireWire (IEEE 1394) Controller, OHCI";
            default:
                return "Serial Bus Controller, FireWire (IEEE 1394) Controller";
            }
        case 0x1:
            return "Serial Bus Controller, ACCESS Bus Controller";
        case 0x2:
            return "Serial Bus Controller, SSA";
        case 0x3: // USB Controller
            switch (prog_if) {
            case 0x0:
                return "Serial Bus Controller, USB Controller, UHCI Controller";
            case 0x10:
                return "Serial Bus Controller, USB Controller, OHCI Controller";
            case 0x20:
                return "Serial Bus Controller, USB Controller, EHCI (USB2) Controller";
            case 0x30:
                return "Serial Bus Controller, USB Controller, XHCI (USB3) Controller";
            case 0x80:
                return "Serial Bus Controller, USB Controller, Unspecified";
            case 0xFE:
                return "Serial Bus Controller, USB Controller, USB Device (Not a host controller)";
            default:
                return "Serial Bus Controller, USB Controller";
            }
        case 0x4:
            return "Serial Bus Controller, Fibre Channel";
        case 0x5:
            return "Serial Bus Controller, SMBus Controller";
        case 0x6:
            return "Serial Bus Controller, InfiniBand Controller";
        case 0x7: // IPMI Interface
            switch (prog_if) {
            case 0x0:
                return "Serial Bus Controller, IPMI Interface, SMIC";
            case 0x1:
                return "Serial Bus Controller, IPMI Interface, Keyboard Controller Style";
            case 0x2:
                return "Serial Bus Controller, IPMI Interface, Block Transfer";
            default:
                return "Serial Bus Controller, IPMI Interface";
            }
        case 0x8:
            return "Serial Bus Controller, SERCOS Interface (IEC 61491)";
        case 0x9:
            return "Serial Bus Controller, CANbus Controller";
        case 0x80:
            return "Serial Bus Controller, Other";
        default:
            return "Serial Bus Controller, Unknown";
        }

    case 0xD: // Wireless Controller
        switch (sub_class) {
        case 0x0:
            return "Wireless Controller, iRDA Compatible Controller";
        case 0x1:
            return "Wireless Controller, Consumer IR Controller";
        case 0x10:
            return "Wireless Controller, RF Controller";
        case 0x11:
            return "Wireless Controller, Bluetooth Controller";
        case 0x12:
            return "Wireless Controller, Broadband Controller";
        case 0x20:
            return "Wireless Controller, Ethernet Controller (802.1a)";
        case 0x21:
            return "Wireless Controller, Ethernet Controller (802.1b)";
        case 0x80:
            return "Wireless Controller, Other";
        default:
            return "Wireless Controller, Unknown";
        }

    case 0xE: // Intelligent Controller
        switch (sub_class) {
        case 0x0:
            return "Intelligent Controller, I20";
        default:
            return "Intelligent Controller, Unknown";
        }

    case 0xF: // Satellite Communication Controller
        switch (sub_class) {
        case 0x1:
            return "Satellite Communication Controller, Satellite TV Controller";
        case 0x2:
            return "Satellite Communication Controller, Satellite Audio Controller";
        case 0x3:
            return "Satellite Communication Controller, Satellite Voice Controller";
        case 0x4:
            return "Satellite Communication Controller, Satellite Data Controller";
        default:
            return "Satellite Communication Controller, Unknown";
        }

    case 0x10: // Encryption Controller
        switch (sub_class) {
        case 0x0:
            return "Encryption Controller, Network and Computing Encryption/Decryption";
        case 0x10:
            return "Encryption Controller, Entertainment Encryption/Decryption";
        case 0x80:
            return "Encryption Controller, Other";
        default:
            return "Encryption Controller, Unknown";
        }

    case 0x11: // Signal Processing Controller
        switch (sub_class) {
        case 0x0:
            return "Signal Processing Controller, DPIO Modules";
        case 0x1:
            return "Signal Processing Controller, Performance Counters";
        case 0x10:
            return "Signal Processing Controller, Communication Synchronizer";
        case 0x20:
            return "Signal Processing Controller, Signal Processing Management";
        case 0x80:
            return "Signal Processing Controller, Other";
        default:
            return "Signal Processing Controller, Unknown";
        }

    case 0x12:
        return "Processing Accelerator";
    case 0x13:
        return "Non-Essential Instrumentation";
    case 0x40:
        return "Co-Processor";
    case 0xFF:
        return "Unassigned Class (Vendor specific)";

    default:
        return "Unknown Device Class";
    }
}
