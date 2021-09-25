#ifndef __VFS_H__
#define __VFS_H__

#include "fs/path.h"
#include "fs/fs.h"
#include "utils/vector.h"

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

struct MountedFS
{
    Path mount_path;
    FS* fs;
};

class VFS
{
public:
    // ------------------- Methods with long docs -------------------
    /**
     * @brief Mounts the given fs in the given path.
     * 
     * @param where  Where to mount. For example - "/dev/", "/"...
     * @param device What device does this FS use? (For example - "/dev/sda").
     * @param what   The filesystem to mount.
     */
    void mount_fs(Path where, Path device, FS* what);

    // ------------------- Regular Methods -------------------
    int open(const char* path, int oflag, ...);
    int openat(int fd, const char *path, int oflag, ...);

    uint64_t pread(int fildes, void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t read(int fildes, void *buf, uint64_t nbyte);

    uint64_t pwrite(int fildes, const void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t write(int fildes, const void *buf, uint64_t nbyte);

private:
    // ------------------- Member Variables -------------------
    Vector<MountedFS> mounted_fss; // All of the currently mounted file systems
};

#endif // __VFS_H__