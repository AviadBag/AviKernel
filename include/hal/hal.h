#ifndef __HAL_H__
#define __HAL_H__

#include "hal/drivers/driver.h"

class HAL
{
public:
    static HAL* get_instance();

    void initialize();

    HAL(HAL &other) = delete; // Should not be copied
    void operator=(const HAL &) = delete; // Should not be assigned

private:
    HAL(); // It's a singleton; It's constructor should be private.

    static HAL* instance;
};

#endif // __HAL_H__