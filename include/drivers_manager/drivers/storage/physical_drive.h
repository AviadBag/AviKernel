#ifndef __DRIVE_H__
#define __DRIVE_H__

#include <stdint.h>

class PhysicalDrive {
public:
    PhysicalDrive(uint32_t sector_size, uint64_t number_of_sectors);

    uint32_t get_sector_size();
    uint64_t get_number_of_sectors();

    uint64_t get_size_by(); // Bytes
    uint64_t get_size_kb();
    uint64_t get_size_mb();
    uint64_t get_size_gb();

private:
    uint32_t sector_size;
    uint64_t number_of_sectors;
};

#endif // __DRIVE_H__
