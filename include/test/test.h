#ifndef TEST_H
#define TEST_H

#include "kernel/panic.h"

#define ASSERT_EQUAL(exp1, exp2)                                                                    \
    {                                                                                               \
        if (exp1 != exp2)                                                                           \
            panic("ASSERT_EQUAL() -> Assertion failed! %s:%d->%s()", __FILE__, __LINE__, __func__); \
    }

#define ASSERT_BIGGER_THAN(exp1, exp2)                                                                    \
    {                                                                                                     \
        if (exp1 <= exp2)                                                                                 \
            panic("ASSERT_BIGGER_THAN() -> Assertion failed! %s:%d->%s()", __FILE__, __LINE__, __func__); \
    }

#define ASSERT_NOT_ZERO(exp)                                                                           \
    {                                                                                                  \
        if (exp == 0)                                                                                  \
            panic("ASSERT_NOT_ZERO() -> Assertion failed! %s:%d->%s()", __FILE__, __LINE__, __func__); \
    }

#define ASSERT_NOT_NULL(exp)                                                                           \
    {                                                                                                  \
        if (exp == 0)                                                                                  \
            panic("ASSERT_NOT_NULL() -> Assertion failed! %s:%d->%s()", __FILE__, __LINE__, __func__); \
    }

#endif