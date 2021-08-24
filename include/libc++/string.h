#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

class String {
public:
    // ----------------------- Constructors -----------------------
    String();
    String(const String& str);
    String(const char* s);
    String(const char* s, size_t n);
    String(size_t n, char c);

    // ----------------------- Operators -----------------------
    String& operator=(const String& str);
    String& operator=(const char* s);
    String& operator=(char c);

    String& operator+=(const String& str);
    String& operator+=(const char* s);
    String& operator+=(char c);

    char& operator[](size_t pos);
    const char& operator[](size_t pos) const;

    // ----------------------- Destructor -----------------------
    virtual ~String();

    // ----------------------- Methods -----------------------
    const char* c_str() const;
    size_t size() const;
    size_t length() const;
    bool empty() const;
    void clear();
    char& at(size_t pos);
    const char& at(size_t pos) const;
    char& back();
    const char& back() const;
    char& front();
    const char& front() const;

private:
    void initialize_from_c_string(const char* str);
    void initialize_from_char(char c);
    void append_c_string(const char* str);
    void append_char(char c);

    char* actual_string = nullptr;

    size_t _size = 0;
};

// ----------------------- Another Operators -----------------------
String operator+(const String& lhs, const String& rhs);
String operator+(const String& lhs, const char* rhs);
String operator+(const char* lhs, const String& rhs);
String operator+(const String& lhs, char rhs);
String operator+(char lhs, const String& rhs);

bool operator==(const String& lhs, const String& rhs);
bool operator==(const char* lhs, const String& rhs);
bool operator==(const String& lhs, const char* rhs);

bool operator!=(const String& lhs, const String& rhs);
bool operator!=(const char* lhs, const String& rhs);
bool operator!=(const String& lhs, const char* rhs);

#endif // __STRING_H__