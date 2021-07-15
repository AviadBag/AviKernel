#include "drivers/screen/text_output.h"
#include "drivers/screen/vga_text.h"

void TextOutput::putchar(char c) 
{
    VgaText::putchar(c);
}