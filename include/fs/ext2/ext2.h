#ifndef _DEVFS_H
#define _DEVFS_H

#include "fs/fs.h"
#include "fs/path.h"

#include <stdint.h>

class Ext2 : public FS
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
};

#endif