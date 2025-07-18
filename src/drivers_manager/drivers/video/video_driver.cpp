#include "drivers_manager/drivers/video/video_driver.h"
#include "kernel/mm/virtual_mgr/virtual_mgr.h"

#include <stdint.h>

#define FRAMEBUF_AVAIL (1 << 12)
#define VIDEO_MEMORY_ADDR 0xC0400000

VideoDriver::VideoDriver()
{
    this->video_memory = (uint32_t *)VIDEO_MEMORY_ADDR;
}

void VideoDriver::set_multiboot_info(multiboot_info_t *multiboot_info)
{
    this->multiboot_info = multiboot_info;
}

void VideoDriver::setup_driver_and_device()
{
    // Map physical video memory given by multiboot to VIDEO_MEMORY_ADDR.
    uint64_t framebuffer_size = multiboot_info->framebuffer_pitch * multiboot_info->framebuffer_height;
    // This formula gives you (framebuffer_size / VMMGR_PAGE_SIZE) rounded UP.
    size_t framebuffer_frames = (framebuffer_size + VMMGR_PAGE_SIZE - 1) / VMMGR_PAGE_SIZE;
    VirtualMgr::map_range((virtual_addr)video_memory, (physical_addr)multiboot_info->framebuffer_addr, framebuffer_frames, true);
}

bool VideoDriver::exist()
{
    // Make sure there is framebuffer info
    if (!(multiboot_info->flags & FRAMEBUF_AVAIL))
        return false;

    // Make sure we are on direct RGB video mode
    if (multiboot_info->framebuffer_type != 1)
        return false;

    // We only support 32 bits pixels
    if (multiboot_info->framebuffer_bpp != 32)
        return false;

    return true;
}

void VideoDriver::put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t pixel = 0;
    pixel |= (r << multiboot_info->framebuffer_red_field_position);
    pixel |= (g << multiboot_info->framebuffer_green_field_position);
    pixel |= (b << multiboot_info->framebuffer_blue_field_position);

    *(video_memory + y * (multiboot_info->framebuffer_pitch / 4) + x) = pixel;
}

uint32_t VideoDriver::get_screen_width()
{
    return multiboot_info->framebuffer_width;
}

uint32_t VideoDriver::get_screen_height()
{
    return multiboot_info->framebuffer_height;
}
