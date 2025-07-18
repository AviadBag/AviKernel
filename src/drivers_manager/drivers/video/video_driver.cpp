#include "drivers_manager/drivers/video/video_driver.h"

void VideoDriver::set_multiboot_info(multiboot_info_t *multiboot_info)
{
    this->multiboot_info = multiboot_info;
}

void VideoDriver::setup_driver_and_device()
{
}

bool VideoDriver::exist()
{
    return true;
}

void VideoDriver::put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
}