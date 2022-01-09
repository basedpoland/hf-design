#define IN_PART_DECL
#include "part-list.hpp"

unsigned djb2::operator()(const char* str) const
{
    unsigned hash = 5381;

    unsigned c;
    while ((c = (unsigned char)(*str++)) != '\0')
        hash = ((hash << 5) + hash) + c;

    return hash;
}

bool chars_equal::operator()(const char* a, const char* b) const
{
    for (; *a == *b && *a; a++, b++)
        (void)0;

    return !*a && !*b;
}
