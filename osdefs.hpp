#pragma once

#ifndef _WIN32
#   include <sysexits.h>
#else
#   undef EX_SOFTWARE
#   undef EX_USAGE
#   define EX_SOFTWARE      70
#   define EX_USAGE         64
#endif

