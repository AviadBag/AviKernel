#include "fs/path.h"

#include <cstdio.h>

Path::Path(String s) 
{
    if (!is_legal(s))
    {
        legal = false;
        return;
    }

    fill_vector(s);
}

void Path::fill_vector(String _s) 
{
    const char* str = _s.c_str();

    // Skip the first slash
    str++;

    // For every part
    String s;
    while (*str)
    {
        if (*str == '/')
        {
            // Prepare for the next part
            parts.append(s);
            s.clear();
        }
        else
        {
            s += *str;
        }
        
        str++;
    }

    if (s.size()) // If the last one was not a slash
        parts.append(s);

    printf("Parts: \n");
    for (int i = 0; i < parts.size(); i++)
    {
        printf("parts[%d] = %s\n", i, parts.get(i).c_str());
    }
}

bool Path::is_legal() 
{
    return legal;
}

bool Path::is_legal(String s) 
{
    // A path cannot be empty...
    if (s.size() == 0)
        return false;

    // A path cannot be only one char - because then it is either '/' or somethings else, which is illegal
    if (s.size() == 1)
        return false;

    // Every path must start from the root directory.
    if (s[0] != '/')
        return false;

    return true;
}
