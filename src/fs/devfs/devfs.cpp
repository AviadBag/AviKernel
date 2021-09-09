#include "fs/devfs/devfs.h"

#include <cstdio.h>

/*
Mapping:
    /sda - first drive
    /sdb - second drive
    ...
*/

void DevFS::mount([[gnu::unused]] int what) {}

void DevFS::umount() {}

void DevFS::read(Path path, size_t count, size_t offset, char* buf) 
{
    for (int i = 0; i < path.get_depth(); i++)
        printf("%s\n", path.get_part(i));
}
