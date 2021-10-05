#include "fs/vfs/vfs.h"
#include "kernel/panic.h"

#include <posix/errno.h>
#include <cstdio.h>

VFS::VFS()
{
    // Descriptors 0, 1, 2 are reserved
    for (int i = 0; i <= 2; i++)
        file_descriptors[i].in_use = true;
}

void VFS::mount_fs(Path where, Path device, FS *what)
{
    if (device.is_folder() && !device.is_root())
        panic("VFS::mount_fs() -> Given a folder as a device!");

    if (!where.is_folder())
        panic("VFS::mount_fs() -> Where to mount must be a folder!");

    MountedFS mfs;
    mfs.mount_path = where;
    mfs.fs = what;
    what->mount(device);
    mounted_fss.append(mfs);
}

FS *VFS::get_fs(Path path)
{
    size_t max = 0;
    FS *fs;

    Path debug_path;

    for (int i = 0; i < mounted_fss.size(); i++)
    {
        MountedFS mounted_fs = mounted_fss.get(i);

        if (mounted_fs.mount_path.get_depth() <= path.get_depth())
        {
            bool found = true;
            for (int j = 0; j < mounted_fs.mount_path.get_depth(); j++)
            {
                if (mounted_fs.mount_path.get_part(i) != path.get_part(i))
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                size_t n = mounted_fs.mount_path.get_depth() + 1;
                if (n > max)
                {
                    max = n;
                    fs = mounted_fs.fs;
                    debug_path = mounted_fs.mount_path;
                }
            }
        }
    }

    return max == 0 ? nullptr : fs;
}

int VFS::open(const char *path, int oflag, ...)
{
    FS *fs = get_fs(path);
    if (!fs)
    {
        errno = ENOENT;
        return -1;
    }

    bool exist = fs->file_exist(path);

    /* -------- Treat the various cases regarding the file existance. -------- */
    // Case 1 - O_CREAT and O_EXCL = fail if file exist.
    if ((oflag & O_CREAT) && (oflag & O_EXCL) && exist)
    {
        errno = EEXIST;
        return -1;
    }
    // Case 2 - O_CREAT is clear and the file does not exist; or path is empty.
    if ((!(oflag & O_CREAT) && !exist) || !(*path))
    {
        errno = ENOENT;
        return -1;
    }

    // Create the file if needed
    if (O_CREAT && !exist)
    {
        fs_status_code code = fs->create_file(path);
        switch (code)
        {
        case FS_NOT_ENOUGH_MEMORY:
            panic("FS::open() -> Not enough memory!");
            break;
        case FS_UNSUPPORTED_OPERATION:
            errno = EACCES;
            return -1;
        case FS_OK:
            break;
        default:
            panic("FS::open() -> unimplemented return code while trying to create a file");
        }
    }

    return -1;
}
