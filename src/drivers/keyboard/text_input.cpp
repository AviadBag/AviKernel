#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/text_input.h"

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
	if (!shift) // Shift disables me.
		if (islower(c)) // We can make this char upper!
			c = toupper(c);
	

	return c;
}

char TextInput::apply_shift(char c)
{
	// For a-z, shift is like caps-lock. But when caps lock is on, then shift disables caps lock.
	if (islower(c)) // We can make this char upper!
	{
		if (!caps_lock) 
			c = toupper(c);
	}
	else
	{
		switch (c)
		{
		case '1':
			c = '!';
			break;
		case '2':
			c = '@';
			break;
		case '3':
			c = '#';
			break;
		case '4':
			c = '$';
			break;
		case '5':
			c = '%';
			break;
		case '6':
			c = '^';
			break;
		case '7':
			c = '&';
			break;
		case '8':
			c = '*';
			break;
		case '9':
			c = '(';
			break;
		case '0':
			c = ')';
			break;
		case '-':
			c = '_';
			break;
		case '=':
			c = '+';
			break;
		case '`':
			c = '~';
			break;
		case ',':
			c = '<';
			break;
		case '.':
			c = '>';
			break;
		case '/':
			c = '?';
			break;
		case '\'':
			c = '"';
			break;
		case '\\':
			c = '|';
			break;
		case '[':
			c = '{';
			break;
		case ']':
			c = '}';
			break;
		case ';':
			c = ':';
			break;
		}
	}

	return c;
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