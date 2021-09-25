#include "fs/vfs/vfs.h"

void VFS::mount_fs(Path where, Path device, FS* what) 
{
    MountedFS mfs;
    mfs.mount_path = where;
    mfs.fs = what;
    what->mount(device);
    mounted_fss.append(mfs);
}
