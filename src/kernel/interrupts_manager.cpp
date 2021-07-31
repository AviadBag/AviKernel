#include "kernel/interrupts_manager.h"

void InterruptsManager::initialize() 
{

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
