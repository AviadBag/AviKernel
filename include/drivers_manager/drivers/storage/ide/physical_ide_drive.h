#ifndef __IDEDRIVE_H__
#define __IDEDRIVE_H__

#include "drivers_manager/drivers/storage/physical_drive.h"

class PhysicalIDEDrive : public PhysicalDrive
{
public:
    PhysicalIDEDrive(uint8_t channel, uint8_t drive_in_channel, bool supports_lba, bool uses_48_bits_mode, uint32_t sector_size, uint64_t number_of_sectors);

    uint8_t get_channel            ();
    uint8_t get_drive_in_channel   ();
    bool    get_supports_lba       ();
    bool    get_uses_48_bits_mode  ();

private:
    uint8_t channel;
    uint8_t drive_in_channel;
    bool    supports_lba;
    bool    uses_48_bits_mode;
};

#endif // __IDEDRIVE_H__
