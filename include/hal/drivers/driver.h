#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver
{
public:
    virtual void initialize() = 0;

protected:
    bool get_initialized();
    
    bool initialized = false;
};

#endif // __DRIVER_H__