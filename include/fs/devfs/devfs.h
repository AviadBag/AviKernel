#ifndef _DEVFS_H
#define _DEVFS_H

#include "fs/fs.h"
#include "fs/path.h"
#include "utils/vector.h"

#include <stdint.h>

/* Used for io() function. (See it's docs) */
enum devfs_operation { DEVFS_OPR_READ, DEVFS_OPR_WRITE };

class DevFS : public FS
{
public:
	// ------------------- Methods Overriden -------------------
	virtual void mount(int what) override;
    virtual void umount() override;

    virtual fs_status_code read(Path path, size_t count, size_t offset, char* buf) override;
    virtual fs_status_code write(Path path, size_t count, size_t offset, char* buf) override;

    virtual fs_status_code create_file(Path path) override;
    virtual fs_status_code delete_file(Path path) override;
    virtual fs_status_code list_files(Path path, Vector<Path>*) override;

private:
    // ------------------- Regular Methods -------------------
    fs_status_code storage_drive_read(Path path, size_t count, size_t offset, char* buf); // <path> must be a legal storage drive path. Unexpected results if else!

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
     * @return fs_status_code 
     */
    fs_status_code io(devfs_operation operation, Path path, size_t count, size_t offset, char* buf);

    // ------------------- Member Variables -------------------
    Vector<Path> root_dir; // Will contain /sda, /sdb...
};

#endif
