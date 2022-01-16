#pragma once
#include "log.hpp"
#include "defs.hpp"
#include <limits>
#include <tuple>

namespace hf::design {

template<typename t> t string_to_type(const char* str, char** endptr);

extern template int string_to_type<int>(const char* str, char** endptr);
extern template float string_to_type<float>(const char* str, char** endptr);

enum class range_behavior : unsigned char { min, max, same };

template<typename t> std::tuple<t, t, bool> parse_range_(const char* str, range_behavior r);

extern template std::tuple<int, int, bool> parse_range_<int>(const char* str, range_behavior r);
extern template std::tuple<float, float, bool> parse_range_<float>(const char* str, range_behavior r);

template<typename t>
struct range final
{
    t min = std::numeric_limits<t>::min();
    t max = std::numeric_limits<t>::max();
    range_behavior r;

    constexpr bool check(t x) const { return x >= min && x <= max; }
    explicit range(range_behavior r = range_behavior::min);
    range(t min, t max, range_behavior r = range_behavior::min);
    void parse(int c, const char* str);
};

extern template struct range<float>;
extern template struct range<int>;

} // namespace hf::design
