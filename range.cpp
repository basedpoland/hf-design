#include "range.hpp"

namespace hf::design {

template<typename t>
std::tuple<t, t, bool> parse_range_(const char* str, range_behavior r)
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
        case range_behavior::same: return { x, x, true  };
        case range_behavior::max:  return { min, x, true };
        case range_behavior::min:  return { x, max, true };
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

template std::tuple<int, int, bool> parse_range_<int>(const char* str, range_behavior r);
template std::tuple<float, float, bool> parse_range_<float>(const char* str, range_behavior r);

} // namespace hf::design
