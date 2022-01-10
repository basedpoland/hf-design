#pragma once
#include <cstdio>

void abort(void);

#define debug_out_(pfx, ...) ([&]() {       \
    if constexpr (sizeof((pfx)) > 1)        \
        fputs((pfx), stderr);               \
    fprintf(stderr, __VA_ARGS__);           \
    fputs("\n", stderr);                    \
    fflush(stderr);                         \
}())

#define WARN(...)   debug_out_("warning: ", __VA_ARGS__)
#define ERR(...)    debug_out_("error: ", __VA_ARGS__)
#define INFO(...)   debug_out_("", __VA_ARGS__)
#define BUG(...)    ((void)debug_out_("BUG: ", __VA_ARGS__), ::abort())
