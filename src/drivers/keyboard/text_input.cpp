#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/text_input.h"

#include <cstdio.h>
#include <cctype.h>

FixedQueue<char> TextInput::queue;
bool TextInput::shift, TextInput::caps_lock;

void TextInput::initialize()
{
	shift = caps_lock = false;

	Keyboard::set_press_extended_char_listener(TextInput::on_press_extended_char);
	Keyboard::set_release_extended_char_listener(TextInput::on_release_extended_char);
}

char TextInput::getchar()
{
	while (queue.empty())
		; // Block until there is a char

	char c = queue.dequeue();
	if (shift)
		c = apply_shift(c);
	if (caps_lock)
		c = apply_caps_lock(c);

	return c;
}

char TextInput::apply_caps_lock(char c)
{
	if (islower(c)) // We can make this char upper!
	{
		c = toupper(c);
	}

	return c;
}

char TextInput::apply_shift(char c)
{
	apply_caps_lock(c); // For a-z, shift is like caps-lock.
}

void TextInput::on_press_extended_char(ExtendedChar ec)
{
	switch (ec.as_avi_charcode())
	{
	case KEY_LSHIFT:
	case KEY_RSHIFT:
		shift = true;
		break;
	case KEY_CAPSLOCK:
		caps_lock = !caps_lock;
		break;

	default:
		break;
	}

	if (ec.printable())
		queue.enqueue(ec.as_regular_char());
}

void TextInput::on_release_extended_char(ExtendedChar ec)
{
	switch (ec.as_avi_charcode())
	{
	case KEY_LSHIFT:
	case KEY_RSHIFT:
		shift = false;
		break;

	default:
		break;
	}
}