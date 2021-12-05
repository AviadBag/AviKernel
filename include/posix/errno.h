#ifndef __ERRNO_H__
#define __ERRNO_H__

#define ENOENT 1
#define EEXIST 2
#define EACCES 3
#define EMFILE 4
#define EBADF 5
#define EOVERFLOW 6
#define EISDIR 7

int get_errno();
void set_errno(int);
#define errno (get_errno())

#endif // __ERRNO_H__