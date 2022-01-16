#pragma once
#include <cerrno>
#include <cstring>
#include <limits>
#include <tuple>

namespace hf::design {

enum class range_behavior : unsigned char { min, max, same };
template<typename t> struct conv_for_type;

template<typename t>
inline t string_to_type(const char* str, char** endptr)
{
    errno = 0;
    return conv_for_type<t>::conv(str, endptr);
}

template<> struct conv_for_type<float>
{
    static float conv(const char* str, char** endptr) { return std::strtof(str, endptr); }
};
template<> struct conv_for_type<int>
{
    static int conv(const char* str, char** endptr) { return std::strtol(str, endptr, 10); }
};

template<typename t> std::tuple<t, t, bool> parse_range_(const char* str, range_behavior r);

extern template std::tuple<int, int, bool> parse_range_<int>(const char* str, range_behavior r);
extern template std::tuple<float, float, bool> parse_range_<float>(const char* str, range_behavior r);

} // namespace hf::design
