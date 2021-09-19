#ifndef _CTYPE_H
#define _CTYPE_H

#define islower(c) ((c) >= 'a' && (c) <= 'z')
#define isupper(c) ((c) >= 'A' && (c) <= 'Z')
#define tolower(c) (isupper(c) ? (c) + 'a' - 'A' : (c))
#define toupper(c) (islower(c) ? (c) + 'A' - 'a' : (c))
#define isspace(c) (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\r' ? 1 : 0)

#endif