#ifndef _FAT32_H
#define _FAT32_H

#include "fs/fs.h"

// This is an implementation of the FAT 32 file system

class FAT32 : public FS
{
public:
	virtual void mount(int what); 
	virtual void umount();

	virtual void read  (char* path, int count, char* buf);
	virtual void write (char* path, int count, char* buf);
	virtual void append(char* path, int count, char* buf);
}

#endif
