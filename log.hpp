#pragma once
#include <cstdio>

#define debug_out(pfx, ...) [&]() { \
    fprintf(stderr, "[%c] ", pfx);  \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "\n");          \
    fflush(stderr);                 \
}()

#define debug2(...) debug_out('d', __VA_ARGS__)
#define debug(...)  debug_out('D', __VA_ARGS__)
#define info(...)   debug_out('I', __VA_ARGS__)
#define warn(...)   debug_out('W', __VA_ARGS__)
#define err(...)    debug_out('E', __VA_ARGS__)
