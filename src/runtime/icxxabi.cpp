#include "runtime/icxxabi.h"

#ifdef __cplusplus
extern "C" {
#endif

atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

extern void* __dso_handle; // Attention! Optimally, you should remove the '= 0' part and define this in your asm script.

int __cxa_atexit(void (*f)(void*), void* objptr, void* dso)
{
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS) {
        return -1;
    };
    __atexit_funcs[__atexit_func_count].destructor_func = f;
    __atexit_funcs[__atexit_func_count].obj_ptr = objptr;
    __atexit_funcs[__atexit_func_count].dso_handle = dso;
    __atexit_func_count++;
    return 0; /*I would prefer if functions returned 1 on success, but the ABI says...*/
};

void __cxa_finalize(void* f)
{
    uarch_t i = __atexit_func_count;
    if (!f) {
        /*
         * According to the Itanium C++ ABI, if __cxa_finalize is called without a
         * function ptr, then it means that we should destroy EVERYTHING MUAHAHAHA!!
         *
         * TODO:
         * Note well, however, that deleting a function from here that contains a __dso_handle
         * means that one link to a shared object file has been terminated. In other words,
         * We should monitor this list (optional, of course), since it tells us how many links to
         * an object file exist at runtime in a particular application. This can be used to tell
         * when a shared object is no longer in use. It is one of many methods, however.
         **/
        // You may insert a prinf() here to tell you whether or not the function gets called. Testing
        // is CRITICAL!
        while (i--) {
            if (__atexit_funcs[i].destructor_func) {
                /* ^^^ That if statement is a safeguard...
                 * To make sure we don't call any entries that have already been called and unset at runtime.
                 * Those will contain a value of 0, and calling a function with value 0
                 * will cause undefined behaviour. Remember that linear address 0,
                 * in a non-virtual address space (physical) contains the IVT and BDA.
                 *
                 * In a virtual environment, the kernel will receive a page fault, and then probably
                 * map in some trash, or a blank page, or something stupid like that.
                 * This will result in the processor executing trash, and...we don't want that.
                 **/
                (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            };
        };
        return;
    };

    while (i--) {
        /*
         * The ABI states that multiple calls to the __cxa_finalize(destructor_func_ptr) function
         * should not destroy objects multiple times. Only one call is needed to eliminate multiple
         * entries with the same address.
         *
         * FIXME:
         * This presents the obvious problem: all destructors must be stored in the order they
         * were placed in the list. I.e: the last initialized object's destructor must be first
         * in the list of destructors to be called. But removing a destructor from the list at runtime
         * creates holes in the table with unfilled entries.
         * Remember that the insertion algorithm in __cxa_atexit simply inserts the next destructor
         * at the end of the table. So, we have holes with our current algorithm
         * This function should be modified to move all the destructors above the one currently
         * being called and removed one place down in the list, so as to cover up the hole.
         * Otherwise, whenever a destructor is called and removed, an entire space in the table is wasted.
         **/
        if (__atexit_funcs[i].destructor_func == f) {
            /*
             * Note that in the next line, not every destructor function is a class destructor.
             * It is perfectly legal to register a non class destructor function as a simple cleanup
             * function to be called on program termination, in which case, it would not NEED an
             * object This pointer. A smart programmer may even take advantage of this and register
             * a C function in the table with the address of some structure containing data about
             * what to clean up on exit.
             * In the case of a function that takes no arguments, it will simply be ignore within the
             * function itself. No worries.
             **/
            (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            __atexit_funcs[i].destructor_func = 0;

            /*
             * Notice that we didn't decrement __atexit_func_count: this is because this algorithm
             * requires patching to deal with the FIXME outlined above.
             **/
        };
    };
};

#ifdef __cplusplus
};
#endif