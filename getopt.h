#pragma once

#ifdef __cplusplus
extern "C" {
#endif
extern const char* musl_optarg;
extern int musl_optind, musl_opterr, musl_optopt;
int musl_getopt(int argc, const char* const* argv, const char* optstring);
#ifdef __cplusplus
}
#endif
