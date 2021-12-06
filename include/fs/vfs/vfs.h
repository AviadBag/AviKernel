#ifndef __VFS_H__
#define __VFS_H__

#include "fs/path.h"
#include "fs/fs.h"
#include "utils/vector.h"

#include <stdint.h>

#define VFS_OPEN_FILES_MAX 200

// First 4 bits of flags
#define O_RDONLY 0b1
#define O_WRONLY 0b10
#define O_RDWR 0b100

// Starting from bit 4
#define O_APPEND 0b1000
#define O_DIRECTORY 0b10000
#define O_CREAT 0b100000
#define O_EXCL 0b1000000

struct MountedFS
{
    Path mount_path;
    FS *fs;
};

struct FileDesc
{
    Path file_path;      /* The path of the file itself */
    uint64_t position;   /* The pointer position inside of the file */
    bool read;           /* Is it open for reading? */
    bool write;          /* Is it open for writing? */
    bool append;         /* Is it open for appending? (Only valid if write=true) */
    uint64_t size;       /* The size of the file, in bytes */
    bool in_use = false; /* Is this descriptor realy in use? */
};

class VFS
{
public:
    // ------------------- Constructor -------------------
    VFS();

    // ------------------- Methods with long docs -------------------
    /**
     * @brief Mounts the given fs in the given path.
     * 
     * @param where  Where to mount. For example - "/dev/", "/"... - MUST BE A DIRECTORY. PANICS IF NOT.
     * @param device What device does this FS use? (For example - "/dev/sda"). - MUST NOT BE A DIRECTORY. PANICS IF NOT.
     *               Give "/" if a device is not needed!
     * @param what   The filesystem to mount.
     */
    void mount_fs(Path where, Path device, FS *what);

    // ------------------- Regular Methods -------------------
    int open(const char *path, int oflag, ...);
    int openat(int fd, const char *path, int oflag, ...);

    uint64_t pread(int fildes, void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t read(int fildes, void *buf, uint64_t nbyte);

    uint64_t pwrite(int fildes, const void *buf, uint64_t nbyte, uint64_t offset);
    uint64_t write(int fildes, const void *buf, uint64_t nbyte);

private:
    // ------------------- Regular Methods -------------------
    bool get_mounted_fs(Path, MountedFS *); // Gives the MountedFS object that holds this path; Returns false if there is not such mounted FS.
    int allocate_descriptor();              // Return -1 if there is no a free descriptor
    void free_descriptor(int desc);         // <desc> must be in use; panics else.

    // ------------------- Member Variables -------------------
    Vector<MountedFS> mounted_fss;                 // All of the currently mounted file systems
    FileDesc file_descriptors[VFS_OPEN_FILES_MAX]; // All of the file descriptors
};

#endif // __VFS_H__