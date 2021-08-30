#include "fs/path.h"

Path::Path(String s) 
{
    if (!is_legal(s))
    {
        legal = false;
        return;
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
