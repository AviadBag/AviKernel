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
    actual_string = new char[1];
    CHECK_ALLOCATION(actual_string);
    *actual_string = '\0';
}

string::string(const string& str) : string(str.c_str()) {}

string::string(const char* s) 
{
    size_t length = strlen(s);
    actual_string = new char[length + 1];
    CHECK_ALLOCATION(actual_string);
    strcpy(actual_string, s);
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