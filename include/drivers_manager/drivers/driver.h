#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver {
public:
    virtual ~Driver() {};

    virtual void setup_driver_and_device() = 0; // Undefined results if called when exist() returns false.
    virtual bool exist() = 0; // Does this device/s exist? CAN BE CALLED BEFORE setup_driver_and_device()!
};

#endif // __DRIVER_H__