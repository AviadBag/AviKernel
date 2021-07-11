#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/text_input.h"
#include <cstdio.h>

FixedQueue<ExtendedChar> TextInput::queue;

void TextInput::initialize()
{
	Keyboard::set_new_extended_char_listener(TextInput::on_new_extended_char);
}

char TextInput::getchar()
{
	while (queue.empty())
		; // Block until there is a char

	return queue.dequeue().as_regular_char();
}

void TextInput::on_new_extended_char(ExtendedChar ec)
{
	if (ec.printable())
		queue.enqueue(ec);
}
