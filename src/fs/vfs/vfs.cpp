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

int VFS::allocate_descriptor()
{
    int desct = -1;

    // Find the first unused descriptor
    for (int i = 0; i < VFS_OPEN_FILES_MAX; i++)
    {
        if (!file_descriptors[i].in_use)
        {
            desct = i;
            break;
        }
    }

    return desct;
}

void VFS::free_descriptor(int desct)
{
    if (!file_descriptors[desct].in_use)
        panic("VFS::free_descriptor() -> trying to free a descriptor which is not in use! (descriptor %d)", desct);

    file_descriptors[desct].in_use = false;
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

bool VFS::get_mounted_fs(Path path, MountedFS *m)
{
    size_t max = 0;

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
                    *m = mounted_fs;
                }
            }
        }
    }

    return max != 0;
}

int VFS::open(const char *path_str, int oflag, ...)
{
    // I must declare the variables at the beginning, because else - it does not allow me to use 'goto'.
    int desct;
    bool exists;
    fs_status_code code;
    MountedFS mounted_fs;
    Path path = path_str;

    if (path.is_folder())
        panic("FS::open() -> FS DOES NOT SUPPORT FOLDER OPENING!");

    // Allocate the descriptor!
    desct = allocate_descriptor();
    if (desct == -1) // There is no a free descriptor
    {
        set_errno(EMFILE);
        goto exit_err;
    }
    file_descriptors[desct].in_use = true;

    // Retrieve the corresponding filesystem
    if (!get_mounted_fs(path, &mounted_fs))
    {
        set_errno(ENOENT);
        goto exit_err_cleanup;
    }

    // Make the path start from root. (For example: "/dev/sda" => "/sda")
    for (int i = 0; i < mounted_fs.mount_path.get_depth(); i++)
        path.remove_part(0);

    // Does this file exist?
    exists = mounted_fs.fs->file_exist(path);

    /* -------- Treat the various cases regarding the file existance. -------- */
    // Case 1 - O_CREAT and O_EXCL = fail if file exist.
    if ((oflag & O_CREAT) && (oflag & O_EXCL) && exists)
    {
        set_errno(EEXIST);
        goto exit_err_cleanup;
    }
    // Case 2 - O_CREAT is clear and the file does not exist; or path is empty.
    if ((!(oflag & O_CREAT) && !exists) || !(*path_str))
    {
        set_errno(ENOENT);
        goto exit_err_cleanup;
    }

    // Create the file if needed
    if (O_CREAT && !exists)
    {
        code = mounted_fs.fs->create_file(path);
        switch (code)
        {
        case FS_NOT_ENOUGH_MEMORY:
            panic("FS::open() -> Not enough memory!");
            break;
        case FS_UNSUPPORTED_OPERATION:
            set_errno(EACCES);
            goto exit_err_cleanup;
        case FS_OK:
            break;
        default:
            panic("FS::open() -> unimplemented return code while trying to create a file");
        }
    }

    // Fill the data in the descriptor
    file_descriptors[desct].file_path = path_str;
    file_descriptors[desct].position = 0; // The beginning of the file
    file_descriptors[desct].read = (oflag & O_RDONLY) || (oflag & O_RDWR);
    file_descriptors[desct].write = (oflag & O_WRONLY) || (oflag & O_RDWR);
    file_descriptors[desct].append = (oflag & O_APPEND);
    code = mounted_fs.fs->get_file_size(path_str, &file_descriptors[desct].size);
    switch (code)
    {
    case FS_NOT_ENOUGH_MEMORY:
        panic("FS::open() -> Not enough memory!");
        break;
    case FS_UNSUPPORTED_OPERATION:
        set_errno(EACCES);
        goto exit_err_cleanup;
    case FS_OK:
        break;
    default:
        panic("FS::open() -> unimplemented return code while trying to create a file");
    }

    return desct;

// Special exists
exit_err_cleanup:
    free_descriptor(desct);
exit_err:
    return -1;
}
