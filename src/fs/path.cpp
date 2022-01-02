#include "fs/path.h"

#include <cstdio.h>

Path::Path(String s)
{
    if (s.empty_or_whitespaces()) // If the path is empty
        s += '/';                 // Then the path points to the root

    if (s[0] != '/') // If the path is relative
        s = '/' + s; // Then make it absolute. (Relative pathes are not supported yet)

    fill_vector(s);
    folder = (s.back() == '/');
}

Path::Path(const char *str) : Path(String(str)) {}

Path::Path() : Path("/") {}

bool Path::operator==(const Path &other)
{
    return other.to_string() == to_string();
}

void Path::fill_vector(String _s)
{
    const char *str = _s.c_str();

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

int Path::get_number_of_parts() const
{
    return parts.size();
}

String Path::to_string() const
{
    String s = "/";
    for (int i = 0; i < get_number_of_parts(); i++)
    {
        s += get_part(i);
        if (is_folder() || i < get_number_of_parts() - 1) // If it is a folder or this is not the last part.
            s += '/';
    }

    return s;
}

bool Path::is_root() const
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

void Path::remove_part(int index)
{
    parts.remove(index); // vector.remove panics if out of range, so we are ok.
}

String Path::get_part(int i) const
{
    return parts.get(i);
}

int Path::get_depth() const
{
    // If it is a folder - then we do not count the file itself as part of the depth
    return is_folder() ? parts.size() : parts.size() - 1;
}

bool Path::is_folder() const
{
    return folder;
}