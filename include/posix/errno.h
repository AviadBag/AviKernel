#ifndef __ERRNO_H__
#define __ERRNO_H__

#define ENOENT 1 /* No such file or directory */
#define EEXIST 2 /* File already exists */
#define EACCES 3
#define EMFILE 4 /* To many open files */
#define EBADF 5
#define EOVERFLOW 6
#define EISDIR 7 /* This is a directory, and a file is required */
#define ENOMEM 8 /* Not enough memory */
#define ENOTSUP 9 /* Operation is not supported */
#define ENOSPC 10 /* Not enough space on disk */

int get_errno();
void set_errno(int);
#define errno (get_errno())

#endif // __ERRNO_H__