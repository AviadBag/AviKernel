#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

namespace std
{
    class string
    {
    public:
        // ----------------------- Constructors -----------------------
        string();
        string (const string& str);
        string (const char* s);
        string (const char* s, size_t n);
        string (size_t n, char c);

        // ----------------------- Operators -----------------------
        string& operator= (const string& str);
        string& operator= (const char* s);
        string& operator= (char c);

        // ----------------------- Destructor -----------------------
        virtual ~string();

        // ----------------------- Methods -----------------------
        const char* c_str() const;

    private:
        void initialize_from_c_string(const char* str);
        void initialize_from_char(char c);

        char* actual_string = nullptr;
    };
}

#endif // __STRING_H__