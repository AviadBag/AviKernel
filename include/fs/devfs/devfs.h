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

    virtual void read(Path path, size_t count, size_t offset, char* buf) override;
    virtual void write(Path path, size_t count, size_t offset, char* buf) override;

    virtual void create_file(Path path) override;
    virtual void delete_file(Path path) override;
    virtual Vector<Path> list_files(Path path) override;
};

#endif
