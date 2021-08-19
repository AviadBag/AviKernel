#include "drivers_manager/drivers/storage/ide/ide_drive.h"

IDEDrive::IDEDrive(uint8_t channel, uint8_t drive_in_channel, uint32_t sector_size, uint64_t number_of_sectors)
    : Drive(sector_size, number_of_sectors)
{
    this->channel = channel;
    this->drive_in_channel = drive_in_channel;
}

uint8_t IDEDrive::get_drive_in_channel() 
{
    return drive_in_channel;
}

uint8_t IDEDrive::get_channel() 
{
    return channel;
}
