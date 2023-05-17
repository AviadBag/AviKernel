#include "test/tester.h"
#include "test/test.h"

#include "fs/fs.h"
#include "fs/devfs/devfs.h"
#include "fs/vfs/vfs.h"

#include "kernel/mm/virtual_mgr/virtual_mgr.h"
#include "kernel/mm/physical_mgr/physical_mgr.h"

#include <string.h>
#include <cstdio.h>
#include <cstring.h>

/**
 * Helps making the log nice and neat.
 * Use INSIDE when entering a new internal testing stage. After finishing - use OUTSIDE.
 * Hint: You probably put INSIDE at the beginning of every TESTING function, and OUTSIDE by it's end.
 */
int tabs = -1; // I don't want the first level to be indented.
#define INSIDE() (tabs++)
#define OUTSIDE() (tabs--)
#define TAB " "

void testing(String what)
{
    for (int i = 0; i < tabs; i++)
        printf(TAB);
    printf("[+] Testing %s...\n", what.c_str());
}

void test_devfs_rw(VFS *vfs)
{
    INSIDE();
    testing("DevFS R/W methods");

    // Create some garbage data
    char *garbage = new char[5000];
    ASSERT_NOT_NULL(garbage);
    memset(garbage, 'C', 4999);
    garbage[4999] = '\0';

    // Get a file descriptor to sda
    int file = vfs->open("/dev/sda", O_RDWR);
    ASSERT_NOT_ZERO(file);

    // Write the garbage somewhere on the disk
    ASSERT_NOT_NULL(vfs->pwrite(file, garbage, 5000, 391010));

    // Read the garbage, make sure it is the same
    char *result = new char[5000];
    ASSERT_NOT_NULL(result);
    ASSERT_NOT_NULL(vfs->pread(file, result, 5000, 391010));
    ASSERT_EQUAL(strcmp(garbage, result), 0);

    OUTSIDE();
}

void test_devfs(VFS *vfs)
{
    INSIDE();
    testing("DevFS");

    test_devfs_rw(vfs);

    OUTSIDE();
}

void test_fs()
{
    INSIDE();
    testing("FS");

    /* Initialize */
    FS *devfs = new DevFS();
    VFS vfs;
    ASSERT_NOT_ZERO(vfs.mount_fs("/dev/", "/", devfs));

    test_devfs(&vfs);

    /* Clean up */
    ASSERT_NOT_ZERO(devfs->umount());
    delete devfs;

    OUTSIDE();
}

void test_vmmgr()
{
    INSIDE();
    testing("Virtual Memory Manager");

    // Allocate some physical blocks
    physical_addr test_p_blocks[100];
    for (int i = 0; i < 100; i++)
    {
        physical_addr block = PhysicalMgr::allocate_block();
        ASSERT_NOT_NULL(block);
        test_p_blocks[i] = block;
    }

    // Map them to 2GB, for example
    virtual_addr test_v_blocks[100];
    for (int i = 0; i < 100; i++)
    {
        test_v_blocks[i] = (virtual_addr)(0x200000 + i * VMMGR_PAGE_SIZE);
        VirtualMgr::map(test_v_blocks[i], test_p_blocks[i], false);
    }

    // And write!
    for (int i = 0; i < 100; i++)
    {
        memset(test_v_blocks[i], 5, VMMGR_PAGE_SIZE);
    }

    OUTSIDE();
}

void test_mm()
{
    INSIDE();
    testing("Memory Managment");

    test_vmmgr();

    OUTSIDE();
}

void Tester::test()
{
    printf("\nBeginning unit testing! Heidad Lameidad!\n");

    test_mm();
    test_fs();
}
