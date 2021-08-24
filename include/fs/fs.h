#ifndef _FS_H
#define _FS_H

class FS {
public:
    virtual ~FS() {};

    // On some File systems, <what> is ignored. (For example, on temp-fs... You only keep the data in the memory...)
    virtual void mount(int what) = 0;
    virtual void umount() = 0;

    virtual void read(char* path, int count, char* buf) = 0;
    virtual void write(char* path, int count, char* buf) = 0;
    virtual void append(char* path, int count, char* buf) = 0;
};

#endif
