#ifndef _MACRO_H
#define _MACRO_H

// Adds <amount> to the void pointer <void_p>
#define ADD_TO_VOID_P(void_p, amount) ((void_p) = (void*)(((char*)(void_p)) + amount))

// Subtracts <amount> from the void pointer <void_p>
#define SUBTRACT_FROM_VOID_P(void_p, amount) ((void_p) = (void*)(((char*)(void_p)) - amount))


#endif