#include "drivers_manager/drivers/storage/ide/storage_ide_compatibility_driver.h"
#include "drivers_manager/drivers/pci/pci_driver.h"
#include "drivers_manager/drivers/pci/pci_device.h"
#include "drivers_manager/drivers_manager.h"
#include "drivers_manager/drivers/storage/ide/ide_drive.h"

#include "utils/vector.h"
#include "utils/io.h"
#include "utils/time.h"

#define SECTOR_SIZE 512

#include <stdint.h>
#include <cstdio.h>

#define ICD_MASTER_DRIVE 0
#define ICD_SLAVE_DRIVE  1

#define ICD_PRIMARY_CHANNEL   0
#define ICD_SECONDARY_CHANNEL 1

#define ICD_NUMBER_OF_CHANNELS 2
#define ICD_DRIVES_PER_CHANNEL 2

// ICD stands for IDE Compatibility Driver
#define ICD_PCI_CLASS_MASS_STORAGE_CONTROLLER 0x1
#define ICD_PCI_SUBCLASS_IDE_CONTROLLER       0x1

// PC stands for Primary Channel, SC stands for Secondary Channel
#define ICD_PC_IN_PCI_MODE_MASK (1 << 0)     // Is the primary channel in PCI mode RIGHT NOW?
#define ICD_PC_SUPPORTS_SWITCH_MASK (1 << 1) // Can the primary channel switch modes?
#define ICD_SC_IN_PCI_MODE_MASK (1 << 2)     // Is the secondary channel in PCI mode RIGHT NOW?
#define ICD_SC_SUPPORTS_SWITCH_MASK (1 << 3) // Can the secondary channel switch modes?

// Commands
#define ICD_IDENTIFY    0xEC
#define ICD_READ_PIO_48 0x24

// Masks
#define ICD_STATUS_BSY (1 << 7)
#define ICD_STATUS_DRQ (1 << 3)
#define ICD_STATUS_ERR (1 << 0)

// Identification space offsets (Offsets in 16 bits size array)
#define ICD_IDENT_CMDS_SET         82
#define ICD_IDENT_CAPABILITIES     49
#define ICD_IDENT_28_BITS_SECTORS  60 // How many sectors there are, when using 28 bits address mode.
#define ICD_IDENT_48_BITS_SECTORS 100 // How many sectors there are, when using 48 bits address mode.

#define IS_SET(data, offset) (((data) & (1 << (offset))) != 0)

StorageIDECompatibilityDriver::StorageIDECompatibilityDriver() 
{
    pci_driver = (PCIDriver*) DriversManager::get_instance()->get_driver(DRIVERS_MANAGER_PCI_DRIVER);
    primary_ide_controller = new IDEController(0x1F0, 0x3F6);
    secondary_ide_controller = new IDEController(0x170, 0x376);

    // TODO: Switch controller to compatibility mode if needed!
}

StorageIDECompatibilityDriver::~StorageIDECompatibilityDriver() 
{
    delete primary_ide_controller;
    delete secondary_ide_controller;
}

IDEController* StorageIDECompatibilityDriver::get_ide_controller(uint8_t channel) 
{
    return channel == ICD_PRIMARY_CHANNEL ? primary_ide_controller : secondary_ide_controller;
}

void StorageIDECompatibilityDriver::disable_interrupts(uint8_t channel) 
{
    get_ide_controller(channel)->write_digital_output_register(0b0010);
}

void StorageIDECompatibilityDriver::ide_select_drive(uint8_t channel, uint8_t drive, bool lba) 
{
    uint8_t tmp = (lba ? 0xE0 : 0xA0);
    uint8_t data = tmp | (drive << 4);
    get_ide_controller(channel)->write_drive_head_register(data);
    Time::sleep(1); // Give the IDE time.
}

void StorageIDECompatibilityDriver::add_drive(uint8_t channel, uint8_t drive, uint16_t* buf) 
{
    bool supports_lba, uses_48_bits_mode;
    uint64_t number_of_sectors;

    uint32_t capabilities = *((uint32_t*)(&(buf[ICD_IDENT_CAPABILITIES])));
    supports_lba = IS_SET(capabilities, 9);

    if (supports_lba)
    {
        uint64_t commands_set = *((uint64_t*)(&(buf[ICD_IDENT_CMDS_SET])));
        uses_48_bits_mode = IS_SET(commands_set, 26);
    } else uses_48_bits_mode = false;

    if (uses_48_bits_mode)
        number_of_sectors = *((uint64_t*)(&(buf[ICD_IDENT_48_BITS_SECTORS])));
    else
        number_of_sectors = buf[ICD_IDENT_28_BITS_SECTORS];

    IDEDrive* drive_to_add = new IDEDrive(channel, drive, supports_lba, uses_48_bits_mode, SECTOR_SIZE, number_of_sectors);
    drives.append(drive_to_add);
    number_of_drives++;
}

void StorageIDECompatibilityDriver::detect_drives() 
{
    for (int channel = 0; channel < ICD_NUMBER_OF_CHANNELS; channel++)
    {
        for (int drive = 0; drive < ICD_DRIVES_PER_CHANNEL; drive++)
        {
            // (I) Select the drive
            ide_select_drive(channel, drive, false);

            // (II) Send the IDENTIFY command
            get_ide_controller(channel)->write_command_register(ICD_IDENTIFY);

            // (III) Does this device exist?
            uint8_t status = get_ide_controller(channel)->read_alternate_status_register();
            if (status == 0) continue; // Does not exist
            
            // There is a device. Let it's BSY clear.
            do
            {
                status = get_ide_controller(channel)->read_alternate_status_register();
            } while (status & ICD_STATUS_BSY);

            // Now - continue polling status. If ERR - the device is not ATA. If DRQ - we can continue.
            bool not_ata = false;
            do
            {
                status = get_ide_controller(channel)->read_alternate_status_register();
                if (status & ICD_STATUS_ERR)
                {
                    not_ata = true;
                    break;
                }
                if (status & ICD_STATUS_DRQ)
                    break;
            } while (true);

            if (not_ata) continue; // This is not ATA - we can continue to the next device.

            uint16_t buf[256];
            get_ide_controller(channel)->read_data_register_buffer(buf, 512);
            add_drive(channel, drive, buf);
        }
    }
}

void StorageIDECompatibilityDriver::setup_driver_and_device() 
{
    // Disable interrupts for both channels
    disable_interrupts(ICD_PRIMARY_CHANNEL);
    disable_interrupts(ICD_SECONDARY_CHANNEL);

    for (int channel = 0; channel < 2; channel++)
    {
        if (!channel_in_compatibility_mode(channel))
        {
            printf("PCI IDE Driver: Channel %d is not in compatibility mode... Switching.\n");
            switch_channel_to_compatibility_mode(channel);
        } else printf("PCI IDE Driver: Channel %d is already in compatibility mode.\n");
    }

    detect_drives();
}

bool StorageIDECompatibilityDriver::exist()
{
    // Check in the PCI if there is an IDE controller
    PCIDevice _ide_controller;
    if (!get_pci_ide_controller(&_ide_controller))
        return false;
    
    // Check if at least one of the channels support compitability mode
    return channel_supports_compatibility_mode(ICD_PRIMARY_CHANNEL) && channel_supports_compatibility_mode(ICD_SECONDARY_CHANNEL);
}

bool StorageIDECompatibilityDriver::channel_supports_compatibility_mode(uint8_t channel) 
{
    return channel_supports_switch(channel) || channel_in_compatibility_mode(channel);
}

void StorageIDECompatibilityDriver::switch_channel_to_compatibility_mode([[gnu::unused]] uint8_t channel) 
{
    uint8_t switch_mask = (channel == ICD_PRIMARY_CHANNEL ? 0b1110 : 0b1011);

    PCIDevice _ide_controller;
    get_pci_ide_controller(&_ide_controller);
    uint8_t prog_if = pci_driver->get_prog_if(_ide_controller);
    prog_if &= switch_mask;
    pci_driver->set_prog_if(_ide_controller, prog_if);
}

bool StorageIDECompatibilityDriver::channel_supports_switch(uint8_t channel) 
{
    uint8_t can_switch_mask = (channel == ICD_PRIMARY_CHANNEL ? 0b0010 : 0b1000);

    PCIDevice _ide_controller;
    get_pci_ide_controller(&_ide_controller);
    uint8_t prog_if = pci_driver->get_prog_if(_ide_controller);
    return prog_if & can_switch_mask;
}

bool StorageIDECompatibilityDriver::channel_in_compatibility_mode(uint8_t channel) 
{
    uint8_t now_mask = (channel == ICD_PRIMARY_CHANNEL ? 0b0000 : 0b0100);
    PCIDevice _ide_controller;
    get_pci_ide_controller(&_ide_controller);
    
    uint8_t prog_if = pci_driver->get_prog_if(_ide_controller);
    return (prog_if & now_mask) == 0;
}

bool StorageIDECompatibilityDriver::get_pci_ide_controller(PCIDevice* device_p) 
{
    Vector<PCIDevice>* pci_devices = pci_driver->get_devices();
    for (int i = 0; i < pci_devices->size(); i++)
    {
        PCIDevice device = pci_devices->get(i);
        uint8_t class_code = pci_driver->get_class_code(device);
        uint8_t sub_class_code = pci_driver->get_sub_class_code(device);
        if (class_code == ICD_PCI_CLASS_MASS_STORAGE_CONTROLLER && sub_class_code == ICD_PCI_SUBCLASS_IDE_CONTROLLER)
        {
            *device_p = device;
            return true;
        }
    }

    return false;
}

void StorageIDECompatibilityDriver::select_drive(int d) 
{
    StorageDriver::select_drive(d);
    IDEDrive* drive = (IDEDrive*) drives.get(selected_drive);
    ide_select_drive(drive->get_channel(), drive->get_drive_in_channel(), false);
}

void StorageIDECompatibilityDriver::read_sector_48_bits(uint64_t lba, char count, char* buffer) 
{
    // Get controller
    IDEDrive* drive = (IDEDrive*) drives.get(selected_drive);
    IDEController* controller = get_ide_controller(drive->get_channel());

    // Wait for drive to be not BSY
    while (controller->read_alternate_status_register() & ICD_STATUS_BSY)
        ;

    // Select current drive as LBA
    ide_select_drive(drive->get_channel(), drive->get_drive_in_channel(), true);

    // Generate LBA values
    uint8_t lba0 = lba >> 0;
    uint8_t lba1 = lba >> 8;
    uint8_t lba2 = lba >> 16;
    uint8_t lba3 = lba >> 24;
    uint8_t lba4 = lba >> 32;
    uint8_t lba5 = lba >> 40;

    // Write LBA values
    controller->write_LBAlo_register_48(lba0, lba3);
    controller->write_LBAmid_register_48(lba1, lba4);
    controller->write_LBIhi_register_48(lba2, lba5);

    // Write sector count
    controller->write_sector_count_register_48(count, 0);

    // Write command
    controller->write_command_register(ICD_READ_PIO_48);

    // Wait for drive to be not BSY
    while (controller->read_alternate_status_register() & ICD_STATUS_BSY)
        ;

    // For every sector
    for (int i = 0; i < count; i++)
    {
        // Wait for DRQ to be set.
        uint8_t status;
        while (((status = controller->read_alternate_status_register()) & ICD_STATUS_DRQ) == 0)
            ;

        // Read sector
        controller->read_data_register_buffer((uint16_t*) buffer, SECTOR_SIZE);
        // Go to next sector
        buffer += 512;
    }
}

void StorageIDECompatibilityDriver::read_sector_28_bits([[gnu::unused]] uint64_t lba, [[gnu::unused]] char count, [[gnu::unused]] char* buffer) 
{
    panic("read_sector_28_bits(): Not implemented!");
}

void StorageIDECompatibilityDriver::read_sector_chs([[gnu::unused]] uint64_t lba, [[gnu::unused]] char count, [[gnu::unused]] char* buffer) 
{
    panic("read_sector_chs(): Not implemented!");
}

void StorageIDECompatibilityDriver::read_sectors(uint64_t lba, char count, char* buffer) 
{
    IDEDrive* drive = (IDEDrive*) drives.get(selected_drive);
    if (drive->get_supports_lba())
    {
        if (drive->get_uses_48_bits_mode())
            read_sector_48_bits(lba, count, buffer);
        else
            read_sector_28_bits(lba, count, buffer);
    } else read_sector_chs(lba, count, buffer);
}

void StorageIDECompatibilityDriver::write_sectors([[gnu::unused]] uint64_t lba, [[gnu::unused]] char count, [[gnu::unused]] char* sector) 
{

}
