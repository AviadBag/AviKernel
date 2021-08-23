#include "kernel/panic.h"

#include <string.h>
#include <cstring.h>

using namespace std;

#define CHECK_ALLOCATION(s) \
{ \
if (!(s)) \
    panic("string: not enough memory!"); \
}

// ----------------------- Constructors -----------------------
string::string() 
{
    initialize_from_char('\0');
}

string::string(const string& str) : string(str.c_str()) {}

string::string(const char* s) 
{
    initialize_from_c_string(s);
}

string::string(const char* s, size_t n) 
{
    actual_string = new char[n + 1];
    CHECK_ALLOCATION(actual_string);
    strncpy(actual_string, s, n);
}

string::string(size_t n, char c) 
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
string& string::operator= (const string& str)
{
    if (actual_string) delete actual_string;
    initialize_from_c_string(str.c_str());
    return *this;
}

string& string::operator= (const char* s) 
{
    if (actual_string) delete actual_string;
    initialize_from_c_string(s);
    return *this;
}

string& string::operator= (char c) 
{
    if (actual_string) delete actual_string;
    initialize_from_char(c);
    return *this;
}

// ----------------------- Destructor -----------------------
string::~string() 
{
    delete actual_string;
}

// ----------------------- Methods -----------------------
const char* string::c_str() const
{
    return actual_string;
}

void string::initialize_from_c_string(const char* s) 
{
    size_t length = strlen(s);
    actual_string = new char[length + 1];
    CHECK_ALLOCATION(actual_string);
    strcpy(actual_string, s);
}

void string::initialize_from_char(char c) 
{
    // todo: do not increment <size> if c == '\0'
    actual_string = new char[1];
    CHECK_ALLOCATION(actual_string);
    *actual_string = c;
}
