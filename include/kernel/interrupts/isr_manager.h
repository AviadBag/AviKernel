#ifndef _ISR_MANAGER_H
#define _ISR_MANAGER_H

#include <stdint.h>

// This module helps to set a register handler. IT IS NOT WRITTEN WITH CLASSES, BECAUSE THE HANDLER MUST BE A C FUNCTION!!!

using isr = void (*)(void* context, uint32_t error_code);

void isr_manager_set_isr(unsigned char interrupt_number, isr i, void* context);

#endif
