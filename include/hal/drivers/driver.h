#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver
{
public:
    virtual void attach() = 0;
    virtual void detach() = 0;
};

#endif // __DRIVER_H__