#pragma once
#include <cstdio>

namespace hf::design {

struct logic_error final {
    const char *file = nullptr;
    int line;
    char msg[128 - sizeof(int) - sizeof(char*)];
};

}

#define ABORT(...) ([&]() {                                     \
    ::hf::design::logic_error _e;                               \
    _e.line = __LINE__;                                         \
    _e.file = __FILE__;                                         \
    std::snprintf(_e.msg, sizeof(_e.msg), __VA_ARGS__);         \
    throw _e; /*NOLINT*/                                        \
}())

#define ASSERT(expr)                                            \
    do {                                                        \
        if (!(expr))                                            \
            ABORT("assertion failed: %s in file %s line %d",    \
                  #expr, __FILE__, __LINE__);                   \
    } while(false)

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

