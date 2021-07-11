#ifndef _TEXT_INPUT_H
#define _TEXT_INPUT_H

#include "extended_char.h"
#include "utils/fixed_queue.h"

class TextInput
{
public:
	static void initialize();
	static char getchar();
	static void on_new_extended_char(ExtendedChar);	
	
private:
	static FixedQueue<ExtendedChar> queue;	

};

#endif
