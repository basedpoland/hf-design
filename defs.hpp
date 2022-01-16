#pragma once

namespace hf::design {
[[noreturn]] void terminate(int status);

struct exit_status final
{
    int code;
    explicit exit_status(int exit_code) : code(exit_code) {}
};

inline void terminate(int status) { throw exit_status(status); }
} // namespace hf::design

#ifndef _WIN32
#   include <sysexits.h>
#else
#   undef EX_SOFTWARE
#   undef EX_USAGE
#   define EX_SOFTWARE      70
#   define EX_USAGE         64
#endif

