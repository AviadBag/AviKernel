#ifndef _VIDEO_DRIVER
#define _VIDEO_DRIVER

#include "drivers_manager/drivers/driver.h"
#include "multiboot/multiboot.h"
#include <stdint.h>

class VideoDriver : public Driver
{
public:
    VideoDriver();

    // MUST be called before anything
    void set_multiboot_info(multiboot_info_t *multiboot_info);

    virtual void setup_driver_and_device();
    virtual bool exist();

    uint32_t get_screen_width();
    uint32_t get_screen_height();

    virtual void put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);

private:
    uint32_t *video_memory;
    multiboot_info_t *multiboot_info;
};

#endif