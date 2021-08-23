#include "kernel/panic.h"

#include <string.h>
#include <cstring.h>

#define CHECK_ALLOCATION(s) \
{ \
if (!(s)) \
    panic("String: not enough memory!"); \
}

// ----------------------- Constructors -----------------------
String::String() 
{
    initialize_from_char('\0');
}

String::String(const String& str) : String(str.c_str()) {}

String::String(const char* s) 
{
    initialize_from_c_string(s);
}

String::String(const char* s, size_t n) 
{
    actual_string = new char[n + 1];
    CHECK_ALLOCATION(actual_string);
    strncpy(actual_string, s, n);
}

String::String(size_t n, char c) 
{
    actual_string = new char[n + 1];
    CHECK_ALLOCATION(actual_string);
    for (int i = 0; i < c; i++)
    {
        actual_string[i] = c;
    }
    actual_string[n] = '\0';
}

// ----------------------- Operators -----------------------
String& String::operator= (const String& str)
{
    if (actual_string) delete actual_string;
    initialize_from_c_string(str.c_str());
    return *this;
}

String& String::operator= (const char* s) 
{
    if (actual_string) delete actual_string;
    initialize_from_c_string(s);
    return *this;
}

String& String::operator= (char c) 
{
    if (actual_string) delete actual_string;
    initialize_from_char(c);
    return *this;
}

// ----------------------- Destructor -----------------------
String::~String() 
{
    delete actual_string;
}

// ----------------------- Methods -----------------------
const char* String::c_str() const
{
    return actual_string;
}

void String::initialize_from_c_string(const char* s) 
{
    size_t length = strlen(s);
    actual_string = new char[length + 1];
    CHECK_ALLOCATION(actual_string);
    strcpy(actual_string, s);
}

void String::initialize_from_char(char c) 
{
    // todo: do not increment <size> if c == '\0'
    actual_string = new char[1];
    CHECK_ALLOCATION(actual_string);
    *actual_string = c;
}
