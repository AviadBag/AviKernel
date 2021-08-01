#include "kernel/interrupts/interrupts_manager.h"
#include "idt.h"
#include "kernel/interrupts/isr_manager.h"

void InterruptsManager::initialize() 
{
    IDT::get_instance()->install();
}

void InterruptsManager::set_isr(unsigned char interrupt_number, isr i, void* context) 
{
    // It is just a wrapper to isr_manager.h, in order to provide an object oriented way to access it.
    isr_manager_set_isr(interrupt_number, i, context);
}

/* ------------------------- Singelton stuff. ------------------------- */
InterruptsManager* InterruptsManager::instance = nullptr;

InterruptsManager::InterruptsManager() {}

InterruptsManager* InterruptsManager::get_instance() 
{
    if (!instance)
        instance = new InterruptsManager;

    return instance;
}
