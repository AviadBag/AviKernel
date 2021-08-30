#ifndef __PATH_H__
#define __PATH_H__

#include "utils/vector.h"

#include <libc++/string.h>

// This class represents a file/folder path.

class Path
{
public:
    Path(String);

    // The directory depth of this path. For example - for /a/b/c.txt - depth=2.
    // And for /a/ - depth=1.
    int get_depth();

    bool is_legal();

private:
    static bool is_legal(String); // Checks if the GIVEN STRING is legal.

    bool legal = true;;
    Vector<String> parts; // Each slesh seperated portion is a "part".
};

#endif // __PATH_H__