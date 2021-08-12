#include "drivers_manager/drivers/screen/graphics/graphics_vba_driver.h"
#include "kernel/panic.h"

#include <cstdio.h>

#define MULTIBOOT_INFO_HAS_VBA_DATA (1 << 11)
#define MUTLIBOOT_INFO_HAS_FB_DATA  (1 << 12)

void GraphicsVBADriver::set_multiboot_info(multiboot_info_t* _multiboot_info) 
{
    multiboot_info = _multiboot_info;
}

void GraphicsVBADriver::setup_driver_and_device() 
{
    if (!multiboot_info)
        panic("GraphicsVBADriver: Call to setup_driver_and_device() before call to set_multiboot_info()!");

    printf("Video Mode: %d\n", multiboot_info->vbe_mode);
}

void GraphicsVBADriver::detach() {} // Empty

bool GraphicsVBADriver::exist() 
{
    if (!multiboot_info)
        panic("GraphicsVBADriver: Call to exist() before call to set_multiboot_info()!");

    return (multiboot_info->flags & MULTIBOOT_INFO_HAS_VBA_DATA) && (multiboot_info->flags & MUTLIBOOT_INFO_HAS_FB_DATA);
}

