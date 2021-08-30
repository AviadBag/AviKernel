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
    folder = s.back() == '/';
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

int Path::get_depth() 
{
    // If it is a folder - then we do not count the file itself as part of the depth
    return is_folder() ? parts.size() : parts.size()-1;
}

bool Path::is_folder() 
{
    return folder;
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

    // Every path must start from the root directory.
    if (s[0] != '/')
        return false;

    return true;
}
