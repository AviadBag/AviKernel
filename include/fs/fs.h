#ifndef _FS_H
#define _FS_H

#include "fs/path.h"
#include "utils/vector.h"

#include <stdint.h>

/**
 * This enum represents the various status codes that can be returned from a fs operation.
 */
enum fs_status_code
{
    FS_OK,                   /* Nothing exceptional happend, everything worked */
    FS_NO_SUCH_FILE,
    FS_NO_SUCH_DIR,
    FS_UNSUPPORTED_OPERATION /* For example - you cannot create a directory in devfs */
};

class FS {
public:
    virtual ~FS() {};

    // On some File systems, <what> is ignored. (For example, on temp-fs... You only keep the data in the memory...)
    virtual void mount(int what) = 0;
    virtual void umount() = 0;

    virtual fs_status_code read(Path path, size_t count, size_t offset, char* buf) = 0;
    virtual fs_status_code write(Path path, size_t count, size_t offset, char* buf) = 0;

    virtual fs_status_code create_file(Path path) = 0;
    virtual fs_status_code delete_file(Path path) = 0;
    virtual fs_status_code list_files(Path path, Vector<Path>*) = 0;
};

#endif
