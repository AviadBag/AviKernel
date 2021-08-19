#ifndef __IDEDRIVE_H__
#define __IDEDRIVE_H__

#include "drivers_manager/drivers/storage/drive.h"

class IDEDrive : public Drive
{
public:
    IDEDrive(uint8_t channel, uint8_t drive_in_channel, uint32_t sector_size, uint64_t number_of_sectors);

    uint8_t get_channel();
    uint8_t get_drive_in_channel();

private:
    uint8_t channel;
    uint8_t drive_in_channel;
};

#endif // __IDEDRIVE_H__