#ifndef __InterruptsManager_MANAGER_H__
#define __InterruptsManager_MANAGER_H__

#include "kernel/interrupts/isr_manager.h" // For the "isr" defintion

// This module manages all of the InterruptsManager stuff. This is a singelton!

class InterruptsManager {
public:
    void initialize();
    void set_isr(unsigned char interrupt_number, isr i, void* context);

    /* ------------------------- Singelton stuff. ------------------------- */
public:
    static InterruptsManager* get_instance();

    InterruptsManager(InterruptsManager& other) = delete; // Should not be cloneable.
    void operator=(const InterruptsManager& other) = delete;

private:
    InterruptsManager(); // This is a singelton - So a private constructor.

    static InterruptsManager* instance;
};

#endif // __InterruptsManager_MANAGER_H__