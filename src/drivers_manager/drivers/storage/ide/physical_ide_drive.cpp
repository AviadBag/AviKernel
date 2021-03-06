#include "drivers_manager/drivers/storage/ide/physical_ide_drive.h"

PhysicalIDEDrive::PhysicalIDEDrive(
    uint8_t channel,
    uint8_t drive_in_channel,
    bool supports_lba,
    bool uses_48_bits_mode,
    uint32_t sector_size,
    uint64_t number_of_sectors)
    : PhysicalDrive(sector_size, number_of_sectors)
{
    this->channel = channel;
    this->drive_in_channel = drive_in_channel;
    this->supports_lba = supports_lba;
    this->uses_48_bits_mode = uses_48_bits_mode;
}

uint8_t PhysicalIDEDrive::get_drive_in_channel()
{
    return drive_in_channel;
}

uint8_t PhysicalIDEDrive::get_channel()
{
    return channel;
}

bool PhysicalIDEDrive::get_supports_lba()
{
    return supports_lba;
}

bool PhysicalIDEDrive::get_uses_48_bits_mode()
{
    return uses_48_bits_mode;
}
