#include "fs/vfs/vfs.h"
#include "kernel/panic.h"

void VFS::mount_fs(Path where, Path device, FS* what) 
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
