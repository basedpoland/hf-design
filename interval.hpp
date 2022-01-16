#pragma once
#include "log.hpp"
#include "defs.hpp"
#include <limits>
#include <tuple>

namespace hf::design {

template<typename t> t string_to_type(const char* str, char** endptr);

extern template int string_to_type<int>(const char* str, char** endptr);
extern template float string_to_type<float>(const char* str, char** endptr);

enum class interval_behavior : unsigned char { min, max, equal };

template<typename t> std::tuple<t, t, bool> parse_interval(const char* str, interval_behavior r);

extern template std::tuple<int, int, bool> parse_interval<int>(const char* str, interval_behavior r);
extern template std::tuple<float, float, bool> parse_interval<float>(const char* str, interval_behavior r);

template<typename t>
struct interval final
{
    t min = std::numeric_limits<t>::min();
    t max = std::numeric_limits<t>::max();
    interval_behavior r;

    constexpr bool check(t x) const { return x >= min && x <= max; }
    explicit interval(interval_behavior r = interval_behavior::min);
    interval(t min, t max, interval_behavior r = interval_behavior::min);
    void parse(int c, const char* str);
};

extern template struct interval<float>;
extern template struct interval<int>;

} // namespace hf::design
