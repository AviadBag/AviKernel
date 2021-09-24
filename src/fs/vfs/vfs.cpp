#include "fs/vfs/vfs.h"

void VFS::mount_fs(Path where, FS* what) 
{
    MountedFS mfs;
    mfs.mount_path = where;
    mfs.fs = what;
    mounted_fss.append(mfs);
}

