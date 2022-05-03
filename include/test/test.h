#ifndef TEST_H
#define TEST_H

#include "kernel/panic.h"

#define ASSERT_EQUAL(exp1, exp2)                                                  \
    {                                                                             \
        if (exp1 != exp2)                                                         \
            panic("Assertion failed! %s:%d->%s()", __FILE__, __LINE__, __func__); \
    }

#endif