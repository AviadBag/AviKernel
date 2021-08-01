#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver
{
public:
    virtual void attach() = 0; // Undefined results if called when exist() returns false.
    virtual void detach() = 0; // Undefined results if attach was not called before.
    virtual bool exist() = 0;  // Does this device/s exist? CAN BE CALLED BEFORE attach()!
};

#endif // __DRIVER_H__