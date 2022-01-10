#pragma once
#include <cstdio>

#define debug_out_(pfx, ...) ([&]() {  \
    if constexpr (sizeof((pfx)) > 1)        \
        fputs((pfx), stderr);               \
    fprintf(stderr, __VA_ARGS__);      \
    fputs("\n", stderr);                    \
    fflush(stderr);                         \
}())

#define warn(...)   debug_out_("warning: ", __VA_ARGS__)
#define err(...)    debug_out_("error: ", __VA_ARGS__)
#define info(...)   debug_out_("", __VA_ARGS__);
