#ifndef __DRIVER_H__
#define __DRIVER_H__

class Driver
{
public:
    void initialize();

protected:
    bool get_initialized();
    
    bool initialized = false;
};

#endif // __DRIVER_H__