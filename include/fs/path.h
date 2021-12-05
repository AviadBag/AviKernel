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
    Path(const char *);

    // ------------------- Operators -------------------
    bool operator==(const Path &other);

    // ------------------- Regular Methods -------------------
    bool is_folder() const;
    bool is_root() const;            // Is path == "/"?
    void go_up();                    // Goes one level up; Must only be called if <folder> = true and <depth> != 0. Panics else.
    void remove_part(int index);     // Removes the given part. (See explenation on docs for get_part()). Panics if index is out of range.
    int get_number_of_parts() const; // How many parts do we have?
    String get_part(int i) const;    // Returns the required part. Panics if i is out of range.
    String to_string() const;

    // ------------------- Methods with long docs -------------------

    /**
     * The directory depth of this path. Root directory is depth 0. For example - for /a/b/c.txt - depth=2.
     * And for /a/ - depth=1.
    */
    int get_depth() const;

private:
    // ------------------- Regular Methods -------------------
    void fill_vector(String s); // Fills the <parts> vector with the parts of the given string; <legal> must be true.

    // ------------------- Member Variables -------------------
    bool folder;          // If false - it is a file
    Vector<String> parts; // Each slesh seperated portion is a "part".
};

#endif // __PATH_H__