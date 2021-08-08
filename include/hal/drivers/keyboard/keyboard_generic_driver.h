#ifndef _KEYBOARD_GENERIC_DRIVER_H__
#define _KEYBOARD_GENERIC_DRIVER_H__

#include "hal/drivers/keyboard/keyboard_driver.h"
#include "hal/drivers/drivers_holder.h"

class KeyboardGenericDriver : public KeyboardDriver {
public:
    KeyboardGenericDriver();
    virtual ~KeyboardGenericDriver();

    virtual void attach() override;
    virtual void detach() override;
    virtual bool exist () override;

    virtual void set_on_press_listener(keyboard_on_press_listener l);
    virtual void set_on_release_listener(keyboard_on_release_listener l);

private:
    DriversHolder drivers_holder;
};

#endif // _KEYBOARD_GENERIC_DRIVER_H__