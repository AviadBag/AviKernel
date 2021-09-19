#ifndef _DEVFS_H
#define _DEVFS_H

#include "fs/fs.h"
#include "fs/path.h"
#include "utils/vector.h"

#include <stdint.h>

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
    // ------------------- Methods Overriden -------------------
    void read_storage_device(String file_name, size_t count, size_t offset, char* buf); // file_name - for example "sda"

    // ------------------- Member Variables -------------------
    Vector<Path> root_dir; // Will contain /sda, /sdb...
};

#endif
