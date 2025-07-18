#include "fs/vfs/vfs.h"
#include "kernel/panic.h"

#include <cstdio.h>
#include <posix/errno.h>

VFS::VFS()
{
    // Descriptors 0, 1, 2 are reserved (POSIX)
    for (int i = 0; i <= 2; i++)
        file_descriptors[i].in_use = true;
}

int VFS::allocate_descriptor()
{
    int desct = -1;

    // Find the first unused descriptor
    for (int i = 0; i < VFS_OPEN_FILES_MAX; i++) {
        if (!file_descriptors[i].in_use) {
            desct = i;
            break;
        }
    }
    file_descriptors[desct].in_use = true;

    return desct;
}

bool VFS::is_legal_descriptor(int desc)
{
    return !(desc >= VFS_OPEN_FILES_MAX || !file_descriptors[desc].in_use);
}

void VFS::free_descriptor(int desct)
{
    if (!file_descriptors[desct].in_use)
        panic("VFS::free_descriptor() -> trying to free a descriptor which is not in use! (descriptor %d)", desct);

    file_descriptors[desct].in_use = false;
}

int VFS::mount_fs(Path where, Path device, FS* what)
{
    if (device.is_folder() && !device.is_root())
        panic("VFS::mount_fs() -> Given a folder as a device!");

    if (!where.is_folder())
        panic("VFS::mount_fs() -> Where to mount must be a folder!");

    MountedFS mfs;
    mfs.mount_path = where;
    mfs.fs = what;
    if (!what->mount(device))
        return 0;
    mounted_fss.append(mfs);

    return 1;
}

bool VFS::get_mounted_fs(Path path, MountedFS* m)
{
    size_t max = 0;

    for (int i = 0; i < mounted_fss.size(); i++) {
        MountedFS mounted_fs = mounted_fss.get(i);

        if (mounted_fs.mount_path.get_depth() <= path.get_depth()) {
            bool found = true;
            for (int j = 0; j < mounted_fs.mount_path.get_depth(); j++) {
                if (mounted_fs.mount_path.get_part(i) != path.get_part(i)) {
                    found = false;
                    break;
                }
            }

            if (found) {
                size_t n = mounted_fs.mount_path.get_depth() + 1;
                if (n > max) {
                    max = n;
                    *m = mounted_fs;
                }
            }
        }
    }

    return max != 0;
}

int VFS::open(const char* path_str, int oflag, ...)
{
    // I must declare the variables at the beginning, because else - it does not allow me to use 'goto'.
    int desct;
    bool exists;
    MountedFS mounted_fs;
    Path path = path_str, trimmed_path;

    if (path.is_folder()) {
        set_errno(EISDIR);
        goto exit_err;
    }

    // Allocate the descriptor!
    desct = allocate_descriptor();
    if (desct == -1) // There is no a free descriptor
    {
        set_errno(EMFILE);
        goto exit_err;
    }

    // Retrieve the corresponding filesystem
    if (!get_mounted_fs(path, &mounted_fs)) {
        set_errno(ENOENT);
        goto exit_err_cleanup;
    }

    // Make the path start from root. (For example: "/dev/sda" => "/sda")
    trimmed_path = path;
    for (int i = 0; i < mounted_fs.mount_path.get_depth(); i++)
        trimmed_path.remove_part(0);

    // Does this file exist?
    exists = mounted_fs.fs->file_exist(trimmed_path);

    /* -------- Treat the various cases regarding the file existance. -------- */
    // Case 1 - O_CREAT and O_EXCL = fail if file exist.
    if ((oflag & O_CREAT) && (oflag & O_EXCL) && exists) {
        set_errno(EEXIST);
        goto exit_err_cleanup;
    }
    // Case 2 - O_CREAT is clear and the file does not exist; or path is empty.
    if ((!(oflag & O_CREAT) && !exists) || !(*path_str)) {
        set_errno(ENOENT);
        goto exit_err_cleanup;
    }

    // Create the file if needed
    if (O_CREAT && !exists) {
        if (!mounted_fs.fs->create_file(trimmed_path))
            return false;
    }

    // Fill the data in the descriptor
    file_descriptors[desct].file_path = path;
    file_descriptors[desct].position = 0; // The beginning of the file
    file_descriptors[desct].read = (oflag & O_RDONLY) || (oflag & O_RDWR);
    file_descriptors[desct].write = (oflag & O_WRONLY) || (oflag & O_RDWR);
    file_descriptors[desct].append = (oflag & O_APPEND);
    if (!mounted_fs.fs->get_file_size(trimmed_path, &file_descriptors[desct].size))
        return false;

    return desct;

// Special exists
exit_err_cleanup:
    free_descriptor(desct);
exit_err:
    return -1;
}

int VFS::close(int fildes)
{
    if (!is_legal_descriptor(fildes)) {
        set_errno(EBADF);
        return -1;
    }

    free_descriptor(fildes);

    return 0;
}

uint64_t VFS::read(int desct, void* buf, uint64_t nbyte)
{
    return io(desct, buf, nbyte, 0, VFS_OPR_READ);
}

uint64_t VFS::pread(int desct, void* buf, uint64_t nbyte, uint64_t offset)
{
    return io(desct, buf, nbyte, offset, VFS_OPR_READ);
}

uint64_t VFS::write(int desct, const void* buf, uint64_t nbyte)
{
    return io(desct, buf, nbyte, 0, VFS_OPR_WRITE);
}

uint64_t VFS::pwrite(int desct, const void* buf, uint64_t nbyte, uint64_t offset)
{
    return io(desct, buf, nbyte, offset, VFS_OPR_WRITE);
}

uint64_t VFS::io(int desct, const void* buf, uint64_t nbyte, uint64_t offset, vfs_operation operation)
{
    // Is <count> == 0?
    if (nbyte == 0)
        return 0;

    // Is it a legal descriptor?
    if (!is_legal_descriptor(desct)) {
        set_errno(EBADF);
        return false;
    }

    // Calculate the actual offset
    uint64_t actual_offset = (offset == 0) ? file_descriptors[desct].position : offset;

    // Is the given file open for reading/writing?
    if ((operation == VFS_OPR_READ && !file_descriptors[desct].read) || (operation == VFS_OPR_WRITE && !file_descriptors[desct].write)) {
        set_errno(EBADF);
        return false;
    }

    // Check for overflow (Only when reading)
    if (operation == VFS_OPR_READ && actual_offset + nbyte > file_descriptors[desct].size) {
        set_errno(EOVERFLOW);
        return false;
    }

    // Get the FS
    MountedFS mounted_fs;
    get_mounted_fs(file_descriptors[desct].file_path, &mounted_fs);

    // Get a trimmed path
    Path trimmed_path = file_descriptors[desct].file_path;
    for (int i = 0; i < mounted_fs.mount_path.get_depth(); i++)
        trimmed_path.remove_part(0);

    // Action!
    uint64_t code;
    if (operation == VFS_OPR_WRITE)
        code = mounted_fs.fs->write(trimmed_path, nbyte, actual_offset, (char*)buf);
    else
        code = mounted_fs.fs->read(trimmed_path, nbyte, actual_offset, (char*)buf);
    if (!code)
        return false;

    // Increment the position in the file descriptor.
    file_descriptors[desct].position = actual_offset + nbyte;

    // If it was a write - recalculate the file size, for it might have changed.
    if (operation == VFS_OPR_WRITE) {
        if (!mounted_fs.fs->get_file_size(trimmed_path, &file_descriptors[desct].size))
            return false;
    }

    return nbyte;
}