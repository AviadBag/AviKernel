#include "fs/path.h"

#include <cstdio.h>

Path::Path(String s) 
{
    if (s.size() == 0) // If the path is empty
        s += '/'; // Then the path points to the root

    if (s[0] != '/') // If the path is relative
        s = '/' + s; // Then make it absolute. (Relative pathes are not supported yet)

    fill_vector(s);
    folder = (s.back() == '/');
}

Path::Path() : Path("/") {}

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
}

bool Path::is_root() 
{
    return get_depth() == 0 && is_folder();
}

void Path::go_up() 
{
    if (get_depth() == 0)
        panic("Path: Cannot go_up() when depth is 0!");
    else if (!is_folder())
        panic("Path: Cannot go_up() when not a folder!");

    parts.pop_back();
}

String Path::get_part(int i) 
{
    return parts.get(i);
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