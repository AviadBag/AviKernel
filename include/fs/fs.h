#ifndef _FS_H
#define _FS_H

#include "fs/path.h"
#include "utils/vector.h"

#include <stdint.h>

#define FS_FILE_EXIST 1
#define FS_FILE_N_EXISTS 2

class FS
{
public:
    virtual ~FS(){};

    // On some File systems, <what> is ignored. (For example, on temp-fs... You only keep the data in the memory...)
    virtual void mount(Path what) = 0;
    virtual void umount() = 0;

    virtual uint64_t read(Path path, uint64_t count, uint64_t offset, char *buf) = 0;
    virtual uint64_t write(Path path, uint64_t count, uint64_t offset, char *buf) = 0;

    virtual uint64_t get_file_size(Path path, uint64_t *size) = 0;
    virtual uint64_t create_file(Path path) = 0;
    virtual uint64_t delete_file(Path path) = 0;
    virtual uint64_t list_files(Path path, Vector<Path> *) = 0;

    virtual int file_exist(Path path) = 0;
};

#endif
