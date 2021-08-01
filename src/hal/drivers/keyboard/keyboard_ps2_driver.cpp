#include "hal/drivers/keyboard/keyboard_ps2_driver.h"
#include "kernel/interrupts/interrupts_manager.h"

#define PS2_INTERRUPT_NUBMER 33

void KeyboardPS2Driver::attach() 
{
    InterruptsManager::get_instance()->set_isr(PS2_INTERRUPT_NUBMER, [](void* context, [[gnu::unused]] uint32_t unused) {
        ((KeyboardPS2Driver*) context)->on_ke_data();
    }, this);
}

void KeyboardPS2Driver::detach() {} // Nothing here

void KeyboardPS2Driver::on_ke_data() 
{
    
}
