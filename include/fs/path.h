#ifndef __PATH_H__
#define __PATH_H__

#include "utils/vector.h"

#include <libc++/string.h>

// This class represents a file/folder path.

class Path
{
public:
    // ------------------- Constructors and Destructor -------------------
    Path();
    Path(String);
    Path(const char*);
    
    // ------------------- Operators -------------------
    bool operator==(const Path& other);

    // ------------------- Regular Methods -------------------
    bool is_folder()   const;
    bool is_root()     const; // Is path == "/"?
    void go_up();             // Goes one level up; Must only be called if <folder> = true and <depth> != 0. Panics else.
    String to_string() const;

    // ------------------- Methods with long docs -------------------
    /**
     * Returns the required part, starting from part <0>. Every string inside of the path is a part.
     * For example - /a/b/c/d.txt:
     *  get_part(0) = "a"
     *  get_part(1) = "b"
     *  get_part(2) = "c"
     *  get_part(3) = "d.txt"
     * I must be in range; panics if it is not.
     */
    String get_part(int i) const;

    /**
     * The directory depth of this path. Root directory is depth 0. For example - for /a/b/c.txt - depth=2.
     * And for /a/ - depth=1.
    */
    int get_depth() const;

private:
    // ------------------- Regular Methods -------------------
    void fill_vector(String s);   // Fills the <parts> vector with the parts of the given string; <legal> must be true.

    // ------------------- Member Variables -------------------
    bool folder;           // If false - it is a file
    Vector<String> parts;  // Each slesh seperated portion is a "part".
    String original_string;
};

#endif // __PATH_H__