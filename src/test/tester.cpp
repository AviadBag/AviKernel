#include "test/tester.h"
#include "test/test.h"

#include "fs/fs.h"
#include "fs/devfs/devfs.h"
#include "fs/vfs/vfs.h"

#include <string.h>
#include <cstdio.h>
#include <cstring.h>

void testing(String what)
{
    printf("[+] Testing %s...\n", what.c_str());
}

void test_devfs_rw()
{
    testing("DevFS R/W methods");

    // Create some garbage data
    char *garbage = new char[5000];
    ASSERT_NOT_NULL(garbage);
    memset(garbage, 'C', 4999);
    garbage[4999] = '\0';

    // Get a file descriptor to sda
    int file = VFS::get_instance()->open("/dev/sda", O_RDWR);
    ASSERT_NOT_ZERO(file);

    // Write the garbage somewhere on the disk
    ASSERT_NOT_NULL(VFS::get_instance()->pwrite(file, garbage, 5000, 391010));

    // Read the garbage, make sure it is the same
    char *result = new char[5000];
    ASSERT_NOT_NULL(result);
    ASSERT_NOT_NULL(VFS::get_instance()->pread(file, result, 5000, 391010));
    ASSERT_EQUAL(strcmp(garbage, result), 0);
}

void test_devfs()
{
    testing("DevFS");

    test_devfs_rw();
}

void test_fs()
{
    testing("FS");

    /* Initialize */
    FS *devfs = new DevFS();
    ASSERT_NOT_ZERO(VFS::get_instance()->mount_fs("/dev/", "/", devfs));

    test_devfs();

    /* Clean up */
    ASSERT_NOT_ZERO(devfs->umount());
    delete devfs;
}

void Tester::test()
{
    printf("\nBeginning unit testing! Heidad Lameidad!\n");

    test_fs();

    printf("Testing done");
}
