#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver
{
public:
    virtual void attach() = 0;
    virtual void detach() = 0;
    virtual bool exist() = 0; // Does this device/s exist? CAN BE CALLED BEFORE attach()!
};

#endif // __DRIVER_H__