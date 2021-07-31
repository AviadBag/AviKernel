#include "kernel/interrupts/interrupts_manager.h"
#include "kernel/interrupts/idt.h"
#include "kernel/interrupts/isr_manager.h"

void InterruptsManager::initialize() 
{
    IDT::get_instance()->install();
}

void InterruptsManager::set_isr(unsigned char interrupt_number, isr_ptr isr_p) 
{
    isr_manager_set_isr(interrupt_number, isr_p);
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
