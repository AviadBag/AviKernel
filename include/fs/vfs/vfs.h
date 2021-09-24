#ifndef __VFS_H__
#define __VFS_H__

#include <stdint.h>

// First 3 bits of flags
#define O_EXEC      1
#define O_RDONLY    2
#define O_RDWR      3
#define O_SEARCH    4
#define O_WRONLY    5

// Starting from bit 4
#define O_APPEND    0b1000
#define O_DIRECTORY 0b10000

class VFS
{
public:
    int open(const char* path, int oflag, ...);
    int openat(int fd, const char *path, int oflag, ...);

    uint64_t pread(int fildes, void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t read(int fildes, void *buf, uint64_t nbyte);

    uint64_t pwrite(int fildes, const void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t write(int fildes, const void *buf, uint64_t nbyte);
};

#endif // __VFS_H__