#include "interval.hpp"
#include <cstdlib>
#include <cerrno>
#include <cstring>

namespace hf::design {

template<typename t> struct conv_for_type;

template<> struct conv_for_type<float>
{
    static float conv(const char* str, char** endptr) { return std::strtof(str, endptr); }
};
template<> struct conv_for_type<int>
{
    static int conv(const char* str, char** endptr) { return std::strtol(str, endptr, 10); }
};

template<typename t>
std::tuple<t, t, bool> parse_interval(const char* str, interval_behavior r)
{
    constexpr auto min = std::numeric_limits<t>::min();
    constexpr auto max = std::numeric_limits<t>::max();
    const char* sep = strchr(str, ':');
    char* endptr;
    errno = 0;
    if (!sep) // lone number
    {
        auto x = string_to_type<t>(str, &endptr);
        if (*endptr || endptr == str || errno)
            return {};
        switch (r)
        {
        default:
        case interval_behavior::equal: return { x, x, true  };
        case interval_behavior::max:  return { min, x, true };
        case interval_behavior::min:  return { x, max, true };
        }

    }
    else if (sep == str) // inf -> x
    {
        auto x = string_to_type<t>(str+1, &endptr);
        if (*endptr || endptr == str+1 || errno)
            return {};
        return { min, x, true };
    }
    else if (sep[1] == '\0') // x -> inf
    {
        auto x = string_to_type<t>(str, &endptr);
        if (endptr != sep || errno)
            return {};
        return { x, max, true };
    }
    else
    {
        auto start = string_to_type<t>(str, &endptr);
        if (endptr != sep || errno)
            return {};
        auto end = string_to_type<t>(sep+1, &endptr);
        if (*endptr || errno || end < start)
            return {};
        return { start, end, true };
    }
}

template<typename t>
void interval<t>::parse(int c, const char* str)
{
    ASSERT(str);

    auto [min_, max_, ok] = parse_interval<t>(str, r);
    if (!ok)
    {
        ERR("invalid interval given to -%c: '%s'", (char)c, str);
        terminate(EX_USAGE);
    }
    min = min_;
    max = max_;
}

template<typename t>
interval<t>::interval(interval_behavior r) : r(r) {}

template<typename t>
interval<t>::interval(t min, t max, interval_behavior r) : min(min), max(max), r(r) {}

template struct interval<float>;
template struct interval<int>;

template std::tuple<int, int, bool> parse_interval<int>(const char* str, interval_behavior r);
template std::tuple<float, float, bool> parse_interval<float>(const char* str, interval_behavior r);

template<typename t>
t string_to_type(const char* str, char** endptr)
{
    errno = 0;
    return conv_for_type<t>::conv(str, endptr);
}

template int string_to_type<int>(const char*, char**);
template float string_to_type<float>(const char*, char**);

} // namespace hf::design
