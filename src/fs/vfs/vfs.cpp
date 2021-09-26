#include "fs/vfs/vfs.h"
#include "kernel/panic.h"

#include <cstdio.h>

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

    if (max == 0)
        printf("Not found a file system for path %s\n", path.to_string().c_str());
    else
        printf("Found a file system mounted on path %s for path %s\n", debug_path.to_string().c_str(), path.to_string().c_str());

    return max == 0 ? nullptr : fs;
}

int VFS::open(const char *path, int oflag, ...)
{
    FS *fs = get_fs(path);
}
