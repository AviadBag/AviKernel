#include "kernel/panic.h"

#include <cstdlib.h>
#include <cstring.h>
#include <string.h>

#define CHECK_ALLOCATION(s)                      \
    {                                            \
        if (!(s))                                \
            panic("String: not enough memory!"); \
    }

// ----------------------- Constructors -----------------------
String::String()
{
    initialize_from_char('\0');
}

String::String(const String& str)
    : String(str.c_str())
{
}

String::String(const char* s)
{
    initialize_from_c_string(s);
}

String::String(const char* s, size_t n)
{
    actual_string = new char[n + 1];
    CHECK_ALLOCATION(actual_string);
    strncpy(actual_string, s, n);

    _size = n;
}

String::String(size_t n, char c)
{
    actual_string = new char[n + 1];
    CHECK_ALLOCATION(actual_string);
    for (int i = 0; i < c; i++) {
        actual_string[i] = c;
    }
    actual_string[n] = '\0';

    _size = n;
}

// ----------------------- Operators -----------------------
String& String::operator=(const String& str)
{
    initialize_from_c_string(str.c_str());
    return *this;
}

String& String::operator=(const char* s)
{
    initialize_from_c_string(s);
    return *this;
}

String& String::operator=(char c)
{
    initialize_from_char(c);
    return *this;
}

String& String::operator+=(const String& str)
{
    append_c_string(str.c_str());
    return *this;
}

String& String::operator+=(const char* s)
{
    append_c_string(s);
    return *this;
}

String& String::operator+=(char c)
{
    append_char(c);
    return *this;
}

char& String::operator[](size_t pos)
{
    return at(pos);
}

const char& String::operator[](size_t pos) const
{
    return at(pos);
}

String operator+(const String& lhs, const String& rhs)
{
    return lhs + rhs.c_str();
}

String operator+(const String& lhs, const char* rhs)
{
    String s = lhs;
    s += rhs;
    return s;
}

String operator+(const char* lhs, const String& rhs)
{
    String s = lhs;
    s += rhs;
    return s;
}

String operator+(const String& lhs, char rhs)
{
    String s = lhs;
    s += rhs;
    return s;
}

String operator+(char lhs, const String& rhs)
{
    String s;
    s = lhs;
    s += rhs;
    return s;
}

bool operator==(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator==(const char* lhs, const String& rhs)
{
    return strcmp(lhs, rhs.c_str()) == 0;
}

bool operator==(const String& lhs, const char* rhs)
{
    return strcmp(lhs.c_str(), rhs) == 0;
}

bool operator!=(const String& lhs, const String& rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const char* lhs, const String& rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const String& lhs, const char* rhs)
{
    return !(lhs == rhs);
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

size_t String::size() const
{
    return _size;
}

size_t String::length() const
{
    return _size;
}

bool String::empty() const
{
    return size() == 0;
}

void String::clear()
{
    initialize_from_char('\0');
}

const char& String::at(size_t pos) const
{
    if (pos > size() - 1)
        panic("String.at(): Index %d out of range!\n", pos);

    return actual_string[pos];
}

char& String::at(size_t pos)
{
    if (pos > size() - 1)
        panic("String.at(): Index %d out of range!\n", pos);

    return actual_string[pos];
}

const char& String::back() const
{
    return actual_string[size() - 1];
}

const char& String::front() const
{
    return actual_string[0];
}

char& String::front()
{
    return actual_string[0];
}

char& String::back()
{
    return actual_string[size() - 1];
}

void String::initialize_from_c_string(const char* s)
{
    if (actual_string)
        delete actual_string;

    size_t length = strlen(s);
    actual_string = new char[length + 1];
    CHECK_ALLOCATION(actual_string);
    strcpy(actual_string, s);

    _size = length;
}

void String::initialize_from_char(char c)
{
    if (actual_string)
        delete actual_string;

    actual_string = new char[1];
    CHECK_ALLOCATION(actual_string);
    *actual_string = c;

    if (c != '\0')
        _size = 1;
    else
        _size = 0;
}

void String::append_c_string(const char* str)
{
    _size = size() + strlen(str);
    actual_string = (char*)realloc(actual_string, size() + 1);
    CHECK_ALLOCATION(actual_string);
    strcat(actual_string, str);
}

void String::append_char(char c)
{
    _size++;
    actual_string = (char*)realloc(actual_string, size() + 1);
    actual_string[size() - 1] = c;
}
