#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

class String
{
public:
    // ----------------------- Constructors -----------------------
    String();
    String (const String& str);
    String (const char* s);
    String (const char* s, size_t n);
    String (size_t n, char c);

    // ----------------------- Operators -----------------------
    String& operator= (const String& str);
    String& operator= (const char* s);
    String& operator= (char c);

    // ----------------------- Destructor -----------------------
    virtual ~String();

    // ----------------------- Methods -----------------------
    const char* c_str() const;
    size_t size() const;
    size_t length() const;

private:
    void initialize_from_c_string(const char* str);
    void initialize_from_char(char c);

    char* actual_string = nullptr;

    size_t _size = 0;
};

#endif // __STRING_H__