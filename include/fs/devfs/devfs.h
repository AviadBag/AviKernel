#ifndef _DEVFS_H
#define _DEVFS_H

#include "fs/fs.h"
#include "fs/path.h"
#include "utils/vector.h"

#include <stdint.h>

/* Used for io() function. (See it's docs) */
enum devfs_operation
{
    DEVFS_OPR_READ,
    DEVFS_OPR_WRITE
};

class DevFS : public FS
{
public:
    // ------------------- Methods Overriden -------------------
    virtual int mount(Path what) override;
    virtual int umount() override;

    virtual uint64_t read(Path path, uint64_t count, uint64_t offset, char *buf) override;
    virtual uint64_t write(Path path, uint64_t count, uint64_t offset, char *buf) override;

    virtual uint64_t get_file_size(Path path, uint64_t *size) override;
    virtual uint64_t create_file(Path path) override;
    virtual uint64_t delete_file(Path path) override;
    virtual uint64_t list_files(Path path, Vector<Path> *) override;

    virtual int file_exist(Path path) override;

private:
    // ------------------- Regular Methods -------------------
    uint64_t storage_drive_read(Path path, uint64_t count, uint64_t offset, char *buf);  // <path> must be a legal storage drive path. Unexpected results if else!
    uint64_t storage_drive_write(Path path, uint64_t count, uint64_t offset, char *buf); // <path> must be a legal storage drive path. Unexpected results if else!

    // ------------------- Methods with Long Docs -------------------
    /**
     * This is the main method of the devfs module. It gets the required operation (r/w), and calls the
     * appropriate function.
     *
     * @param operation What operation to do? (read/write)
     * @param path      The path to the file
     * @param count     How many bytes to read/write?
     * @param offset    From what offset to begin?
     * @param buf       The buffer to write from / read into
     * @return uint64_t
     */
    uint64_t io(devfs_operation operation, Path path, uint64_t count, uint64_t offset, char *buf);

    // ------------------- Member Variables -------------------
    Vector<Path> root_dir; // Will contain /sda, /sdb...
};

#endif
