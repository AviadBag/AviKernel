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

        // ----------------------- Destructor -----------------------
        virtual ~string();

        // ----------------------- Methods -----------------------
        const char* c_str() const;

    private:
        char* actual_string;
    };
}

#endif // __STRING_H__