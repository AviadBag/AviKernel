#ifndef __GRAPHICS_VBA_DRIVER_H__
#define __GRAPHICS_VBA_DRIVER_H__

#include "drivers_manager/drivers/driver.h"
#include "multiboot/multiboot.h"

class GraphicsVBADriver : public Driver
{
public:
    void set_multiboot_info(multiboot_info_t* _multiboot_info); // MUST be called before attach and exist!!!

    virtual void setup_driver_and_device() override;
    virtual bool exist()  override; // Will return false if the multiboot info does not contains the required information.

private:
    multiboot_info_t* multiboot_info = nullptr;
};

#endif // __GRAPHICS_VBA_DRIVER_H__